# RUN WITH ./[Servername] [Port] test_websites

# Returns 200, successful response etag matches
curl -v --header "If-Match: Tue, 01 Feb 2022" http://localhost:18000/example/index.html 

# Returns 404, etag does not match
curl -v --header "If-Match: Tue, 01 Feb 2222" http://localhost:18000/example/index.html 

# Returns 200, file does not match etag
curl -v --header "If-None-Match: Tue, 01 Feb 2222" http://localhost:18000/example/index.html 

# Returns 404, file matches etag 
curl -v --header "If-None-Match: Tue, 01 Feb 2022" http://localhost:18000/example/index.html 

# Returns 200, file has been modified since given date
curl -v --header "If-Modified-Since: Wed, 21 Oct 2002 07:28:00 GMT" http://localhost:18000/example/index.html

# Returns 304, file has not been modified
curl -v --header "If-Modified-Since: Tue, 01 Feb 2022 14:06:23 GMT" http://localhost:18000/example/index.html

# Returns 200, file has not been modified (below: since its last modified date)
curl -v --header "If-UnModified-Since: Tue, 01 Feb 2022 14:06:23 GMT" http://localhost:18000/example/index.html

# Returns 412, precondition failed 
curl -v --header "If-UnModified-Since: Tue, 01 Feb 2000 14:06:23 GMT" http://localhost:18000/example/index.html
