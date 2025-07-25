# $HOME/.profile* or similar files may first set $DEBUGINFOD_URLS.
# If $DEBUGINFOD_URLS is not set there, we set it from system *.url files.
# $HOME/.*rc or similar files may then amend $DEBUGINFOD_URLS.
# See also [man debuginfod-client-config] for other environment variables
# such as $DEBUGINFOD_MAXSIZE, $DEBUGINFOD_MAXTIME, $DEBUGINFOD_PROGRESS.

# Use local variables so we don't need to manually unset them
set --local prefix "/usr/local"

if not set --query DEBUGINFOD_URLS
    set --local files "${prefix}/etc/debuginfod/"*.urls
    set --local DEBUGINFOD_URLS (cat /dev/null $files 2>/dev/null | string replace '\n' ' ')
    if test -n "$DEBUGINFOD_URLS"
        set --global --export DEBUGINFOD_URLS "$DEBUGINFOD_URLS"
    end
end

if not set --query DEBUGINFOD_IMA_CERT_PATH
    set --local files "${prefix}/etc/debuginfod/"*.certpath
    set --local DEBUGINFOD_IMA_CERT_PATH (cat /dev/null $files 2>/dev/null | string replace '\n' ':')
    if test -n "$DEBUGINFOD_IMA_CERT_PATH"
        set --global --export DEBUGINFOD_IMA_CERT_PATH "$DEBUGINFOD_IMA_CERT_PATH"
    end
end
