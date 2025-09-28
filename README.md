Programmed two Raspberry Pis to communicate and exchange drawings in real time over UART through a GUI, 
using my own handshake protocol for synchronisation.

Created two threads: serialise and deserialise, 
with the serialise thread encoding the QT points into bits 
and sending them through its own UART-like communication with my independently made handshake protocol.

https://github.com/user-attachments/assets/219f0a4d-fb86-4e4f-a245-f22d9fb1161b


