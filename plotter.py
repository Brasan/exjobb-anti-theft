import matplotlib.pyplot as plt
import csv

def main():
    OUTFILENAME = "arduino_output_1684229578.csv"
    INFILENAME = "arduino_input_1684229576.csv"
    output = []
    input = []
    ox = []
    oy = []
    oz = []
    ix = []
    iy = []
    iz = []

    with open(OUTFILENAME, 'r') as file:
        csv_read = csv.reader(file, delimiter=',')
        for row in csv_read:
            output = row
            break
    
    output.remove('')   

    with open(INFILENAME, 'r') as file:
        csv_read = csv.reader(file, delimiter=',')
        for row in csv_read:
            input = row
            break
    
    input.remove('')  

    i = 0
    while (i < 420):
        ox.append(float(output[i]))
        oy.append(float(output[i+1]))
        oz.append(float(output[i+2]))
        i+=3
    
    i = 0
    while (i < 420):
        ix.append(float(input[i]))
        iy.append(float(input[i+1]))
        iz.append(float(input[i+2]))
        i+=3
    plt.plot(ix, 'b')
    plt.plot(ox, 'r')
    plt.plot(iy, 'b')
    plt.plot(oy, 'r')
    plt.plot(iz, 'b')
    plt.plot(oz, 'r')
    plt.show() 

if __name__ == "__main__":
    main()