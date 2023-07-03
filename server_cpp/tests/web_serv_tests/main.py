'''
The purpose of this test is to connect to the webserver using python and run a basic connect and send test using a tcp client.
'''
from operator import mod
import os
from os import walk, system, name
import sys
from time import sleep

#################################
host = "localhost"
port = "3000"
################################

def int_only():
    while True:
        try:
            question = int(input("\n> "))
            return question
        except:
            print("Invalid input, please try again")

def clear():
  
    # for windows
    if name == 'nt':
        _ = system('cls')
  
    # for mac and linux(here, os.name is 'posix')
    else:
        _ = system('clear')

def init(path):
    print("Loading tests...")
    imports = next(walk(path), (None, None, []))[2]  # [] if no file
    modules = []
    for test in imports:
        if test == "__init__.py": continue
        test = test[:-3]
        try:
            modules.append(__import__(path + "." + test, globals(), locals(), test, 0))
            print ("Successfully imported", test)
        except ImportError:
            template = "Unable to load {0}. Error:\n{1!r}"
            message = template.format(test, ex.args)
            print (message)

        except Exception as ex:
            template = "An exception of type {0} occurred. Error:\n{1!r}"
            message = template.format(type(ex).__name__, ex.args)
            print (message)

    if len(imports) <= 1: 
        print("No tests were found. Exiting.")
        exit(-1)
    
    ret = []
    for i in imports:
        if i == "__init__.py": continue
        ret.append(path + '.' + i[:-3])
    return modules

def welcome_screen():
                                                                                                                                                                                                                            
    print("""                                                                                                                   
 _____   _____   _____   _____   _____   _____   _____   _____   _____   _____   _____   _____   _____   _____   _____  
|_____| |_____| |_____| |_____| |_____| |_____| |_____| |_____| |_____| |_____| |_____| |_____| |_____| |_____| |_____| 
                                                                                                                        
                                                                                                                        
                                                                                                                        
                                                                                                                        
                                                                                                                        
                                                                                                                        
                                                                                                                        
 _______  _______  ______    __   __  _______  ______      _______  _______  _______  _______  ___   __    _  _______   
|       ||       ||    _ |  |  | |  ||       ||    _ |    |       ||       ||       ||       ||   | |  |  | ||       |  
|  _____||    ___||   | ||  |  |_|  ||    ___||   | ||    |_     _||    ___||  _____||_     _||   | |   |_| ||    ___|  
| |_____ |   |___ |   |_||_ |       ||   |___ |   |_||_     |   |  |   |___ | |_____   |   |  |   | |       ||   | __   
|_____  ||    ___||    __  ||       ||    ___||    __  |    |   |  |    ___||_____  |  |   |  |   | |  _    ||   ||  |  
 _____| ||   |___ |   |  | | |     | |   |___ |   |  | |    |   |  |   |___  _____| |  |   |  |   | | | |   ||   |_| |  
|_______||_______||___|  |_|  |___|  |_______||___|  |_|    |___|  |_______||_______|  |___|  |___| |_|  |__||_______|  
                                                                                                                        
                                                                                                                        
                                                                                                                        
                                                                                                                        
                                                                                                                        
 _____   _____   _____   _____   _____   _____   _____   _____   _____   _____   _____   _____   _____   _____   _____  
|_____| |_____| |_____| |_____| |_____| |_____| |_____| |_____| |_____| |_____| |_____| |_____| |_____| |_____| |_____| 
\n\n\n""")

def test(modules):
    running = True
    while running:
        clear()
        print(f"Please select test:")

        i = 0
        for x in modules:
            i += 1
            mod_name = ""
            try: 
                mod_name = x.get_module_name()
                print(f"{i}: {mod_name}")
            except Exception as ex:
                template = "The test file seems to not have a get_module_name function or it is formatted incorrectly. (An exception of type {0} occurred. Error:\n{1!r})"
                message = template.format(type(ex).__name__, ex.args)
                print (message)
                modules.remove(x)
                i -= 1
        
        inpt = 0
        while True: 
            s = int_only()
            if s > 0 and s <= len(modules):
                inpt = s
                break
        try:
            success = modules[inpt - 1].run(host, port)
        except Exception as ex:
            template = "The test file seems to not have a run function or it is formatted incorrectly. (An exception of type {0} occurred. Error:\n{1!r})"
            message = template.format(type(ex).__name__, ex.args)
            print (message)

        print("\n")

        if success == True: print("Test success!")
        elif success == False: print("Test failed!")

        input("\n\nPress any key to continue running")
        
        
def main():
    welcome_screen()
    modules = init("commands")
    sleep(2)

    test(modules)


if __name__ == "__main__":
    sys.exit(int(main() or 0))