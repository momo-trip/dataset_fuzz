/* This program is licensed under the GNU General Public License, version 2,
 * a copy of which is included with this program.
 *
 * (c) 2000-2001 Michael Smith <msmith@xiph.org>
 * (c) 2008 Michael Gold <mgold@ncf.ca>
 *
 *
 * Simple application to cut an ogg at a specified frame, and produce two
 * output files.
 *
 * last modified: $Id$
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#ifndef _MSC_VER
#include <unistd.h>
#endif

#include "vcut.h"

#include <locale.h>
#include "i18n.h"

#include <inttypes.h>

static void clear_packet(vcut_packet *p)
{
	if(p->packet)
		free(p->packet);
	p->packet = NULL;
}

/* Returns 0 for success, or -1 on failure. */
static void *vcut_malloc(size_t size)
{
	void *ret = malloc(size);
	/* FIXME: libogg will probably crash if one of its malloc calls fails,
	 *        so we can't always catch an OOM error. */
	if(!ret)
		fprintf(stderr, _("Out of memory\n"));
	return ret;
}

/* Returns 0 for success, or -1 on failure. */
static int save_packet(ogg_packet *packet, vcut_packet *p)
{
	clear_packet(p);
	p->length = packet->bytes;
	p->packet = vcut_malloc(p->length);
	if(!p->packet)
		return -1;

	memcpy(p->packet, packet->packet, p->length);
	return 0;
}

static long get_blocksize(vcut_vorbis_stream *vs, ogg_packet *op)
{
	int this = vorbis_packet_blocksize(&vs->vi, op);
	int ret = (this+vs->prevW)/4;
 
	vs->prevW = this;
	return ret;
}

static int update_sync(vcut_state *s)
{
	char *buffer = ogg_sync_buffer(&s->sync_in, 4096);
	int bytes = fread(buffer, 1, 4096, s->in);
	ogg_sync_wrote(&s->sync_in, bytes);
	return bytes;
}

/* Writes pages to the given file, or discards them if file is NULL.
 * Returns 0 for success, or -1 on failure. */
static int write_pages_to_file(ogg_stream_state *stream,
		FILE *file, int flush)
{
	ogg_page page;

	if(flush)
	{
		while(ogg_stream_flush(stream, &page))
		{
			if(!file) continue;
			if(fwrite(page.header,1,page.header_len, file) != page.header_len)
				return -1;
			if(fwrite(page.body,1,page.body_len, file) != page.body_len)
				return -1;
		}
	}
	else
	{
		while(ogg_stream_pageout(stream, &page))
		{
			if(!file) continue;
			if(fwrite(page.header,1,page.header_len, file) != page.header_len)
				return -1;
			if(fwrite(page.body,1,page.body_len, file) != page.body_len)
				return -1;
		}
	}

	return 0;
}


/* Flushes and closes the output stream, leaving the file open.
 * Returns 0 for success, or -1 on failure. */
static int close_output_stream(vcut_state *s)
{
	assert(s->output_stream_open);

	if(write_pages_to_file(&s->stream_out, s->out, 1) != 0)
	{
		fprintf(stderr, _("Couldn't flush output stream\n"));
		return -1;
	}

	ogg_stream_clear(&s->stream_out);
	s->output_stream_open = 0;
	return 0;
}

/* Closes the output file and stream.
 * Returns 0 for success, or -1 on failure. */
static int close_output_file(vcut_state *s)
{
	FILE *out = s->out;
	if(s->output_stream_open && (close_output_stream(s) != 0))
		return -1;

	s->out = NULL;
	if(out && fclose(out) != 0)
	{
		fprintf(stderr, _("Couldn't close output file\n"));
		return -1;
	}

	s->output_filename = NULL;
	s->drop_output = 0;
	return 0;
}

/* Write out the header packets and reference audio packet. */
static int submit_headers_to_stream(vcut_state *s)
{
	vcut_vorbis_stream *vs = &s->vorbis_stream;
	int i;
	for(i=0;i<4;i++)
	{
		ogg_packet p;
		if(i < 3)  /* a header packet */
		{
			p.bytes = vs->headers[i].length;
			p.packet = vs->headers[i].packet;
		}
		else  /* the reference audio packet */
		{
			if (!vs->last_packet.packet) break;
			p.bytes = vs->last_packet.length;
			p.packet = vs->last_packet.packet;
		}

		assert(p.packet);
		p.b_o_s = ((i==0)?1:0);
		p.e_o_s = 0;
		p.granulepos = 0;
		p.packetno = i;

		if (write_packet(s, &p) != 0)
			return -1;
	}
	return 0;
}

/* Opens the given output file; or sets s->drop_output if the filename is ".".
 * Returns 0 for success, or -1 on failure. */
static int open_output_file(vcut_state *s, char *filename)
{
	assert(s->out == NULL);

	if(strcmp(filename, ".") == 0)
	{
		s->out = NULL;
		s->drop_output = 1;
	}

	else
	{
                if(strcmp(filename, "-") == 0)
                        s->out = fdopen(1, "wb");
                else
                        s->out = fopen(filename, "wb");
		s->drop_output = 0;
		if(!s->out) {
			fprintf(stderr, _("Couldn't open %s for writing\n"), filename);
			return -1;
		}
	}
	return 0;
}

/* Opens an output stream; if necessary, opens the next output file first.
 * Returns 0 for success, or -1 on failure. */
static int open_output_stream(vcut_state *s)
{
        int rv;
	if(!s->out && !s->drop_output)
	{
		if(open_output_file(s, s->output_filename)!=0)
			return -1;
	}

	/* ogg_stream_init should only fail if stream_out is null */
	rv = ogg_stream_init(&s->stream_out, ++s->serial_out);
	assert(rv == 0);

	s->output_stream_open = 1;
	return submit_headers_to_stream(s);
}


int original_main(int argc, char **argv)
{
	int ret=0;
	vcut_state state;
	vcut_segment *seg;
	memset(&state, 0, sizeof(state));

	setlocale(LC_ALL, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);

	if(argc<5)
	{
		printf(_("Usage: vcut infile.ogg outfile1.ogg"
				" outfile2.ogg [cutpoint | +cuttime]\n"));
		printf(_("To avoid creating an output file,"
				" specify \".\" as its name.\n"));
		exit(1);
	}

	state.in = fopen(argv[1], "rb");
        if(strcmp(argv[1], "-") == 0)
                state.in = fdopen(0, "rb");
        else
                state.in = fopen(argv[1], "rb");
	if(!state.in) {
		fprintf(stderr, _("Couldn't open %s for reading\n"), argv[1]);
		exit(1);
	}

	state.output_filename = argv[2];
	seg = vcut_malloc(sizeof(vcut_segment));
	if(!seg)
		exit(1);
	seg->cuttime = -1;
	seg->filename = argv[3];
	seg->next = NULL;
	state.next_segment = seg;

	if(strchr(argv[4], '+') != NULL) {
	  if(sscanf(argv[4], "%lf", &seg->cuttime) != 1) {
	    fprintf(stderr, _("Couldn't parse cutpoint \"%s\"\n"), argv[4]);
            exit(1);
	  }
	} else if(sscanf(argv[4], "%" PRId64, &seg->cutpoint) != 1) {
	    fprintf(stderr, _("Couldn't parse cutpoint \"%s\"\n"), argv[4]);
            exit(1);
	}

	if(seg->cuttime >= 0) {
		printf(_("Processing: Cutting at %lf seconds\n"), seg->cuttime);
	} else {
		printf(_("Processing: Cutting at %lld samples\n"),
				(long long)seg->cutpoint);
	}

	/* include the PID in the random seed so two instances of vcut that
	 * run in the same second will use different serial numbers */
	srand(time(NULL) ^ getpid());
	state.serial_out = rand();

	if(vcut_process(&state) != 0)
	{
		fprintf(stderr, _("Processing failed\n"));
		ret = 1;
	}

	vcut_clear(&state);
	fclose(state.in);

	return ret;
}

/* Returns 0 for success, or -1 on failure. */
int process_audio_packet(vcut_state *s,
		vcut_vorbis_stream *vs, ogg_packet *packet)
{
	int bs = get_blocksize(vs, packet);
	long cut_on_eos = 0;
	int packet_done = 0;
	ogg_int64_t packet_start_granpos = vs->granulepos;
	ogg_int64_t gp_to_global_sample_adj;

	if(packet->granulepos >= 0)
	{
		/* If this is the second audio packet, and our calculated
		 * granule position is greater than the packet's, this means
		 * some audio samples must be discarded from the beginning
		 * when decoding (in this case, the Vorbis I spec. requires
		 * that this be the last packet on its page, so its granulepos
		 * will be available). Likewise, if the last packet's
		 * granulepos is less than expected, samples should be
		 * discarded from the end. */

		if(vs->granulepos == 0 && packet->granulepos != bs)
		{
			/* this stream starts at a non-zero granulepos */
			vs->initial_granpos = packet->granulepos - bs;
			if(vs->initial_granpos < 0)
				vs->initial_granpos = 0;
		}
		else if(packet->granulepos != (vs->granulepos + bs)
				&& vs->granulepos > 0 && !packet->e_o_s)
		{
			fprintf(stderr, _("WARNING: unexpected granulepos "
					"%" PRId64 " (expected " "%" PRId64 ")\n"),
					packet->granulepos, (vs->granulepos + bs));
		}
		vs->granulepos = packet->granulepos;
	}
	else if(vs->granulepos == -1)
	{
		/* This is the first non-header packet. The next packet
		 * will start at granulepos 0, or will be the last packet
		 * on its page (as mentioned above). */
		vs->granulepos = 0;

		/* Don't look for a cutpoint in this packet. */
		packet_done = 1;
	}
	else
	{
		vs->granulepos += bs;
	}

	gp_to_global_sample_adj = s->prevstream_samples - vs->initial_granpos;
	while(!packet_done)
	{
		ogg_int64_t rel_cutpoint, rel_sample;
		vcut_segment *segment = s->next_segment;
		if(!segment) break;

		if(segment->cuttime >= 0)
		{
			/* convert cuttime to cutpoint (a sample number) */
			rel_cutpoint = vs->vi.rate * (s->next_segment->cuttime
					- s->prevstream_time);
		}
		else
		{
			rel_cutpoint = (segment->cutpoint - s->prevstream_samples);
		}

		rel_sample = vs->granulepos - vs->initial_granpos;
		if(rel_sample < rel_cutpoint)
			break;

		/* reached the cutpoint */

		if(rel_sample - bs > rel_cutpoint)
		{
			/* We passed the cutpoint without finding it. This could mean
			 * that the granpos values are discontinuous (in which case
			 * we'd have shown an "Unexpected granulepos" error), or that
			 * the cutpoints are not sorted correctly. */
			fprintf(stderr, _("Cutpoint not found\n"));
			return -1;
		}

		if(rel_sample < bs && !s->drop_output)
		{
			fprintf(stderr, _("Can't produce a file starting"
					" and ending between sample positions " "%" PRId64
					" and " "%" PRId64 "\n"),
					packet_start_granpos + gp_to_global_sample_adj - 1,
					vs->granulepos + gp_to_global_sample_adj);
			return -1;
		}

		/* Set it! This 'truncates' the final packet, as needed. */
		packet->granulepos = rel_cutpoint;
		cut_on_eos = packet->e_o_s;
		packet->e_o_s = 1;
		if(rel_cutpoint > 0)
		{
			if(write_packet(s, packet) != 0)
				return -1;
		}
		if(close_output_file(s) != 0)
			return -1;

		vs->samples_cut = rel_cutpoint;
		packet->e_o_s = cut_on_eos;

		s->output_filename = segment->filename;
		s->next_segment = segment->next;
		free(segment);
		segment = NULL;

		if(rel_cutpoint == rel_sample)
		{
			/* There's no unwritten data left in this packet. */
			packet_done = 1;
		}
	}

	/* Write the audio packet to the output stream, unless it's the
	 * reference packet or we cut it at the last sample. */
	if(!packet_done)
	{
		packet->granulepos = vs->granulepos
				- vs->initial_granpos - vs->samples_cut;
		if(packet->granulepos < bs && cut_on_eos
				&& strcmp(s->output_filename, ".") != 0)
		{
			fprintf(stderr, _("Can't produce a file starting between sample"
					" positions " "%" PRId64 " and " "%" PRId64 ".\n"),
					packet_start_granpos + gp_to_global_sample_adj - 1,
					vs->granulepos + gp_to_global_sample_adj);
			fprintf(stderr, _("Specify \".\" as the second output file"
					" to suppress this error.\n"));
			return -1;
		}
		if(write_packet(s, packet) != 0)
			return -1;
	}

	/* We need to save the last packet in the first
	 * stream - but we don't know when we're going
	 * to get there. So we have to keep every packet
	 * just in case. */
	if(save_packet(packet, &vs->last_packet) != 0)
		return -1;

	return 0;
}

/* Writes a packet, opening an output stream/file if necessary.
 * Returns 0 for success, or -1 on failure. */
int write_packet(vcut_state *s, ogg_packet *packet)
{
	int flush;
	if(!s->output_stream_open && open_output_stream(s) != 0)
		return -1;

	/* According to the Vorbis I spec, we need to flush the stream after:
	 *  - the first (BOS) header packet
	 *  - the last header packet (packet #2)
	 *  - the second audio packet (packet #4), if the stream starts at
	 *    a non-zero granulepos */
	flush = (s->stream_out.packetno == 2)
			|| (s->stream_out.packetno == 4 && packet->granulepos != -1)
			|| packet->b_o_s || packet->e_o_s;
	ogg_stream_packetin(&s->stream_out, packet);

	if(write_pages_to_file(&s->stream_out, s->out, flush) != 0)
	{
		fprintf(stderr, _("Couldn't write packet to output file\n"));
		return -1;
	}

	if(packet->e_o_s && close_output_stream(s) != 0)
		return -1;

	return 0;
}

/* Returns 0 for success, or -1 on failure. */
int process_page(vcut_state *s, ogg_page *page)
{
	int headercount;
	int result;
	vcut_vorbis_stream *vs = &s->vorbis_stream;

	if(!s->vorbis_init)
	{
		if(!ogg_page_bos(page))
		{
			fprintf(stderr, _("BOS not set on first page of stream\n"));
			return -1;
		}

		memset(vs, 0, sizeof(*vs));
		vs->serial = ogg_page_serialno(page);
		vs->granulepos = -1;
		vs->initial_granpos = 0;
		ogg_stream_init(&vs->stream_in, vs->serial);
		vorbis_info_init(&vs->vi);
		vorbis_comment_init(&vs->vc);
		s->vorbis_init = 1;
	}
	else if(vs->serial != ogg_page_serialno(page))
	{
		fprintf(stderr, _("Multiplexed bitstreams are not supported\n"));
		return -1;
	}

	/* count the headers */
	for(headercount = 0; headercount < 3; ++headercount)
		if(!vs->headers[headercount].packet) break;

	result = ogg_stream_pagein(&vs->stream_in, page);
	if(result)
	{
		fprintf(stderr, _("Internal stream parsing error\n"));
		return -1;
	}

	while(1)
	{
		ogg_packet packet;
		result = ogg_stream_packetout(&vs->stream_in, &packet);

		if(result==0) break;
		else if(result==-1)
		{
			if(headercount < 3)
			{
				fprintf(stderr, _("Header packet corrupt\n"));
				return -1;
			}
			else
			{
				if(!s->input_corrupt)
					fprintf(stderr, _("Bitstream error, continuing\n"));
				s->input_corrupt = 1;
				continue;
			}
		}

		if(headercount < 3)  /* this is a header packet */
		{
			if(vorbis_synthesis_headerin(&vs->vi, &vs->vc, &packet)<0)
			{
				fprintf(stderr, _("Error in header: not vorbis?\n"));
				return -1;
			}

			if(save_packet(&packet, &vs->headers[headercount]) != 0)
				return -1;

			++headercount;
		}
		else  /* this is an audio (non-header) packet */
		{
			result = process_audio_packet(s, vs, &packet);
			if(result != 0)
				return result;
		}
	}

	if(ogg_page_eos(page))
	{
		if(vs->granulepos >= 0)
		{
			ogg_int64_t samples = vs->granulepos - vs->initial_granpos;
			s->prevstream_samples += samples;
			s->prevstream_time += (double)samples / vs->vi.rate;
		}
		vcut_vorbis_clear(vs);
		s->vorbis_init = 0;
	}

	return 0;
}

/* Returns 0 for success, or -1 on failure. */
int vcut_process(vcut_state *s)
{
	int first_page = 1;
	do
	{
		ogg_page page;
		int result;

		while((result = ogg_sync_pageout(&s->sync_in, &page)) == 1)
		{
			if(process_page(s, &page) != 0)
				return -1;
		}

		if(result < 0 && !s->input_corrupt)
		{
			if(first_page)
			{
				fprintf(stderr, _("Input not ogg.\n"));
				return -1;
			}

			fprintf(stderr, _("Page error, continuing\n"));
			/* continue, but don't print this error again */
			s->input_corrupt = 1;
		}
		first_page = 0;
	}
	while(update_sync(s) > 0);

	if(s->vorbis_init)
	{
		fprintf(stderr, _("WARNING: input file ended unexpectedly\n"));
	}
	else if(s->next_segment)
	{
		fprintf(stderr, _("WARNING: found EOS before cutpoint\n"));
	}
	return close_output_file(s);
}

void vcut_vorbis_clear(vcut_vorbis_stream *vs)
{
	int i;
	clear_packet(&vs->last_packet);

	for(i=0; i < 3; i++)
		clear_packet(&vs->headers[i]);

	vorbis_block_clear(&vs->vb);
	vorbis_dsp_clear(&vs->vd);
	vorbis_info_clear(&vs->vi);
	ogg_stream_clear(&vs->stream_in);
}

/* Full cleanup of internal state and vorbis/ogg structures */
void vcut_clear(vcut_state *s)
{
	if(s->vorbis_init)
	{
		vcut_vorbis_clear(&s->vorbis_stream);
		s->vorbis_init = 0;
	}
	ogg_sync_clear(&s->sync_in);
}


#include <time.h>    /* for time(), localtime(), strftime() */
#include <errno.h>   /* for errno */
#include <sys/stat.h> /* for struct stat, stat() */
#include <unistd.h>  /* for other functions */

// Custom string copy function
char* my_string_copy(const char* src) {
  if (!src) return NULL;
  size_t len = strlen(src) + 1;
  char* copy = malloc(len);
  if (copy) {
      memcpy(copy, src, len);
  }
  return copy;
}

// Version without using strdup
int parse_command_line_from_file(const char* filename, char*** new_argv, int* new_argc, const char* program_name) {
  FILE *input_file;
  char buffer[1024];
  size_t bytes_read;
  char *token_start, *token_end;
  int arg_count = 1; // For argv[0] (program name)
  int i;
  
  // Check if debug logging is enabled
  int enable_logging = (getenv("SHELLGEN_LOG") != NULL);
  
  if (enable_logging) {
      fprintf(stderr, "[PARSE_DEBUG] Opening file: %s\n", filename);
  }
  
  // Open the file
  input_file = fopen(filename, "r");
  if (!input_file) {
      if (enable_logging) {
          fprintf(stderr, "Error opening input file: %s\n", filename);
      }
      return -1;
  }
  
  // Read the file contents
  bytes_read = fread(buffer, 1, sizeof(buffer) - 1, input_file);
  fclose(input_file);
  
  if (enable_logging) {
      fprintf(stderr, "[PARSE_DEBUG] Read %zu bytes\n", bytes_read);
  }
  
  if (bytes_read == 0) {
      if (enable_logging) {
          fprintf(stderr, "Input file is empty or could not be read\n");
      }
      return -1;
  }
  
  buffer[bytes_read] = '\0';  // Null-terminate
  
  if (enable_logging) {
      fprintf(stderr, "[PARSE_DEBUG] File content: '%s'\n", buffer);
  }
  
  // Remove newline characters
  char* newline = strchr(buffer, '\n');
  if (newline) *newline = '\0';
  char* carriage = strchr(buffer, '\r');
  if (carriage) *carriage = '\0';
  
  if (enable_logging) {
      fprintf(stderr, "[PARSE_DEBUG] After newline removal: '%s'\n", buffer);
  }
  
  // Count arguments manually (without strtok)
  char* p = buffer;
  while (*p) {
      // Skip whitespace
      while (*p == ' ' || *p == '\t') p++;
      if (*p == '\0') break;
      
      // Found start of token
      arg_count++;
      
      // Skip to end of token
      while (*p && *p != ' ' && *p != '\t') p++;
  }
  
  if (enable_logging) {
      fprintf(stderr, "[PARSE_DEBUG] Total arguments: %d\n", arg_count);
  }
  
  // Allocate argv array
  *new_argv = (char**)malloc(sizeof(char*) * (arg_count + 1));
  if (*new_argv == NULL) {
      if (enable_logging) {
          fprintf(stderr, "Memory allocation failed\n");
      }
      return -1;
  }
  
  // argv[0] is the actual program name
  (*new_argv)[0] = my_string_copy(program_name);
  if (!(*new_argv)[0]) {
      if (enable_logging) {
          fprintf(stderr, "[PARSE_DEBUG] string copy failed for program_name\n");
      }
      free(*new_argv);
      return -1;
  }
  
  // Parse arguments manually
  p = buffer;
  i = 1;
  while (*p && i < arg_count) {
      // Skip whitespace
      while (*p == ' ' || *p == '\t') p++;
      if (*p == '\0') break;
      
      // Find start and end of token
      token_start = p;
      while (*p && *p != ' ' && *p != '\t') p++;
      token_end = p;
      
      // Calculate token length
      size_t token_len = token_end - token_start;
      
      // Allocate and copy token
      (*new_argv)[i] = malloc(token_len + 1);
      if (!(*new_argv)[i]) {
          if (enable_logging) {
              fprintf(stderr, "[PARSE_DEBUG] malloc failed for token %d\n", i);
          }
          // Cleanup
          for (int j = 0; j < i; j++) {
              free((*new_argv)[j]);
          }
          free(*new_argv);
          return -1;
      }
      
      memcpy((*new_argv)[i], token_start, token_len);
      (*new_argv)[i][token_len] = '\0';
      
      if (enable_logging) {
          fprintf(stderr, "[PARSE_DEBUG] new_argv[%d] = '%s'\n", i, (*new_argv)[i]);
      }
      i++;
  }
  
  (*new_argv)[arg_count] = NULL; // End with NULL
  *new_argc = arg_count;
  
  if (enable_logging) {
      fprintf(stderr, "[PARSE_DEBUG] Successfully parsed %d arguments\n", arg_count);
  }
  
  return 0;
}

int main(int argc, char* argv[]) {
  // Check if debug logging is enabled
  int enable_logging = (getenv("SHELLGEN_LOG") != NULL);
  
  if (enable_logging) {
      fprintf(stderr, "[DEBUG] main() started\n");
      fprintf(stderr, "[DEBUG] argc = %d\n", argc);
      for (int i = 0; i < argc; i++) {
          fprintf(stderr, "[DEBUG] argv[%d] = '%s'\n", i, argv[i]);
      }
  }
  
  // For AFL++ @@ mode when arguments are provided
  if (argc >= 2) {
      if (enable_logging) {
          fprintf(stderr, "[DEBUG] AFL++ mode: reading from file %s\n", argv[1]);
      }
      
      // Read and process arguments from file
      char **new_argv = NULL;
      int new_argc = 0;
      
      if (parse_command_line_from_file(argv[1], &new_argv, &new_argc, argv[0]) == 0) {
          if (enable_logging) {
              fprintf(stderr, "[DEBUG] Successfully parsed %d arguments from file\n", new_argc);
              for (int i = 0; i < new_argc; i++) {
                  fprintf(stderr, "[DEBUG] new_argv[%d] = '%s'\n", i, new_argv[i]);
              }
              fprintf(stderr, "[DEBUG] Calling original_main with %d arguments\n", new_argc);

              // Also record to log file
              FILE *log_file = fopen("fuzzing_log.txt", "a");
              if (log_file) {
                  time_t current_time = time(NULL);
                  char time_str[64];
                  strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&current_time));
                  
                  fprintf(log_file, "[%s] --- Parsed command line arguments from file: %s ---\n", time_str, argv[1]);
                  fprintf(log_file, "[%s] argc = %d\n", time_str, new_argc);
                  for (int i = 0; i < new_argc; i++) {
                      fprintf(log_file, "[%s] argv[%d] = '%s'\n", time_str, i, new_argv[i]);
                  }
                  fprintf(log_file, "[%s] --- End of command line arguments ---\n\n", time_str);
                  fclose(log_file);
              }
              
          }
          
          // Call original_main directly
          //return original_main(new_argc, new_argv);
          
          int result = original_main(new_argc, new_argv);
          if (new_argv) {
            for (int i = 0; i < new_argc; i++) {
                if (new_argv[i]) {
                    free(new_argv[i]);
                }
            }
            free(new_argv);
          }
          return result;
        
      } else {
          if (enable_logging) {
              fprintf(stderr, "[DEBUG] Failed to parse arguments from file\n");
          }
          return 1;
      }
  }
  
  // For normal processing when no arguments are provided
  if (enable_logging) {
      fprintf(stderr, "[DEBUG] Normal mode: calling original_main directly\n");
  }
  
  return original_main(argc, argv);
}

