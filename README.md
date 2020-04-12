# RPC (<span style="color: green"> R</span>emote <span style="color: green"> P</span>rocedure <span style="color: green"> C</span>all)

RPC means invoking a function or procedure which is implemented and run on a remote machine in the network. In other words, in RPC, local application invokes the function which resides on a remote machine. RPC creates an **illusion** as if you are invoking the local function, but in reality that function is remote. Sometimes, we are not able to implement some functions or procedures due to the hardware constraints. It means that the hardware may not supports some features needed by the function. Therefore, the developer is forced to implement the function/procedure in a remote compatible physical machine.

The entire RPC mechanism can be divided into 9 steps:

1. Client program invokes the RPC with arguments.
2. Client program converts the arguments into serialized form of data.(Data Serialization/ Marshaling)
3. Client program ships the serialized data over the network to RPC server.
---(client side)---
4. RPC server receives the serialized data obtained in step 3 from client.
5. RPC server un-marshales the data received in step 4.(De-Serialization)
6. RPC server invokes the actual procedure with arguments.
7. RPC server has to perform serialization of the result.(Data Serialization/ Marshaling)
8. RPC server sends the serialized result back to the client.
---(server side)---
9. client un-marshales the received serialized result. 
