import socket # for socket
import sys
import time


module_name = "Basic Connects (series)"

def get_module_name():
    return module_name

def int_only():
    while True:
        try:
            question = int(input("\n> "))
            return question
        except:
            print("Invalid input, please try again")

def run(host, port_i):
    print("How many connections would you like to run?")
    iterations = int_only()
    for i in range (iterations):
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

    input("Test Complete. Press any key to continue...")
    return True
