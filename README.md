Programmed two Raspberry Pis to communicate and exchange drawings in real time over UART through a GUI, 
using my own handshake protocol for synchronisation.

The GUI was developed using the cross-platform IDE Qt Creator.

Created two threads: serialise and deserialise, 
with the serialise thread encoding the QT points into bits 
and sending them through my own UART-like communication with my independently made handshake protocol.

<img width="595" height="283" alt="HandshakeProtocol" src="https://github.com/user-attachments/assets/9a721c40-91e9-4134-bde6-a1265ddad1f9" />

I had to compress the video demonstration to keep it under GitHub’s maximum size limit, which explains the reduced quality.

For more step-by-step information on the process of making this project, please look at Whiteboard_Logbook.pdf.

https://github.com/user-attachments/assets/219f0a4d-fb86-4e4f-a245-f22d9fb1161b

There is a problem with the program crash, 
and the cause I overlooked from back then was that the memory keeps getting full from the encoded data. 
To fix this, you can de-allocate and allocate the culprit variables in the code.


