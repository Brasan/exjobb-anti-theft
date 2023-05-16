import serial
import matplotlib as plot
import re
import time

def main ():
    DUMPALL = False
    PORTNAME = "COM5"
    datapoints_out = []
    datapoints_in = []
    port = serial.Serial(PORTNAME, 9600)

    print("beginning to read...")
    while(port.is_open): # and buffer is not empty?

        if DUMPALL:
            with open("arduino_data_dump.csv", "a") as file:
                file.write(re.search("[-]*\d+\.\d+", str(port.readline()))[0] + ',')

        output = port.readline()

        if (b"BEGINOUT" in output): #Model output value
            datapoints_out.clear()
            while (True):
                output = port.readline()
                if (b"ENDOUT" in output):
                    print("Done reading output! Read", len(datapoints_out), "datapoints")
                    cleanOutput(datapoints_out)
                    writeToCsv(datapoints_out)
                    
                    break
                datapoints_out.append(str(output))

        if (b"BEGININ") in output: #Accelerometer input values
            datapoints_in = []
            while (True):
                output = port.readline()
                if (b"ENDIN" in output):
                    print("Done reading input! Read", len(datapoints_in), "datapoints")
                    cleanOutput(datapoints_in)
                    writeToCsv(datapoints_in, output=False)
                    
                    break
                datapoints_in.append(str(output))

    print("Finished")

def writeToCsv(data, output = True):
    timestamp = str(time.time_ns() // 1000000000)

    if output:
        filename = "arduino_output_" + timestamp  + ".csv"
    else:
        filename = "arduino_input_" + timestamp  + ".csv"
        
    with open(filename, "x") as file:
        for element in data:
            file.write(element + ',')

def cleanOutput(output):
    for i in range(len(output)):
        output[i] = re.search("[-]*\d+\.\d+", output[i])[0]
        #output[i] = 
                   


if __name__ == "__main__":
    main()