import serial
import matplotlib as plot
import re
import time

def main ():
    datapoints = []
    port = serial.Serial("COM6", 9600)

    print("beginning to read...")
    while(port.is_open): # and buffer is not empty?
        if (b"BEGIN" in port.readline()):
            datapoints = []
            while (True):
                output = port.readline()
                if (b"END" in output):
                    print("Done reading! Read", len(datapoints), "datapoints")
                    cleanOutput(datapoints)
                    writeToCsv(datapoints)
                    
                    break
                datapoints.append(str(output))
    print("Finished")

def writeToCsv(data):
    filename = "arduino_output_" + str(time.time_ns() // 1000000000) + ".csv"
    with open(filename, "x") as file:
        for element in data:
            file.write(element + ',')

def cleanOutput(output):
    for i in range(len(output)):
        output[i] = re.search("\d+\.\d+", output[i])[0]
        #output[i] = 
                   


if __name__ == "__main__":
    main()