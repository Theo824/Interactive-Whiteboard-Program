Programmed two Raspberry Pis to communicate and exchange drawings in real time over UART through a GUI, 
using my own handshake protocol for synchronisation.

The GUI was developed using the cross-platform IDE Qt Creator.

Created two threads: serialise and deserialise, 
with the serialise thread encoding the QT points into bits 
and sending them through its own UART-like communication with my independently made handshake protocol.

I had to compress the video demonstration to keep it under GitHub’s maximum size limit, which explains the reduced quality.

For more step-by-step information on the process of making this project, please look at Whiteboard_Logbook.pdf.

https://github.com/user-attachments/assets/219f0a4d-fb86-4e4f-a245-f22d9fb1161b


