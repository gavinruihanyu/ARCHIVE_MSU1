import socket # for socket
import sys

module_name = "Singular Basic Connect and Disconnect"

def get_module_name():
    return module_name

def run(host, port_i):
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        print ("Socket successfully created")
    except socket.error as err:
        print ("socket creation failed with error %s" %(err))
     
    # default port for socket
    port = int(port_i)
     
    try:
        host_ip = socket.gethostbyname(host)
    except socket.gaierror:
     
        # this means could not resolve the host
        print ("there was an error resolving the host")
        sys.exit()
     
    # connecting to the server
    try:
        s.connect((host_ip, port))
    except TimeoutError:
        print ("Could not connect to server.")
    except Exception as ex:
            template = "An exception of type {0} occurred. Error:\n{1!r}"
            message = template.format(type(ex).__name__, ex.args)
            print (message)
            return False
    else:
        print (f"the socket has successfully connected to {host}:{port}")

    try:
        s.shutdown(socket.SHUT_RDWR)
        s.close()
    except Exception as ex:
        template = "An exception of type {0} occurred. Error:\n{1!r}"
        message = template.format(type(ex).__name__, ex.args)
        print (message)
        return False
    else:
        print (f"the socket has successfully connected to {host}:{port}")
        return True
    
    