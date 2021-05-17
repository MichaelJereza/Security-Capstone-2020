i686-w64-mingw32-g++ -static-libgcc -static-libstdc++ -static exampleMalware.cpp -o exampleMalware.exe

note: the key generation is rather crudely implemented and initialized based off of system time.
    the defender could probably attack the encrytpion based on nonuniform randomness.
      

note: files from crypto-algorithms are from:
  https://github.com/B-Con/crypto-algorithms.git
  I copyed the files into the repo for ease of use (makes compiling way easyer).
  Also I added some comments and print statements for debugging to the files. Otherwise they are exactly the 
  same. "crypto-aglorithms" isn't a completely secure implementation but its good enough for demonstatration. 
  Go to their page for more info on the implementation. 

  

