import socket
import time
import random
import math
import cv2
import sys

DGRAM = 2**16 - 64

class Client:
    def __init__(self, name):
        self.name = name

        self.socket_tcp = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket_udp = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    def connect(self, addr):
        self.addr = addr
        self.socket_tcp.connect(self.addr)

        msg = f"{self.name}"
        self.socket_tcp.send(bytes(msg, 'utf8'))

    def udpsend(self, data):
        size = len(data)
        segments = math.ceil(size / DGRAM)

        start = 0
        while segments:
            end = min(size, start + DGRAM)
            self.socket_udp.sendto(data[start:end], self.addr)

            start = end
            segments -= 1

            res = self.socket_udp.recvfrom(5)


class RandomStreamClient(Client):
    def start_stream(self):
        data = bytearray()
        while True:
            for i in range(0, 640 * 480 * 3):
                data.append(random.randint(0, 255))
            
            self.udpsend(data)
            

class OpencvClient(Client):
    def __init__(self, name):
        Client.__init__(self, name)
        self.cap = cv2.VideoCapture(0)

    def start_stream(self):
        while True:
            ret, frame = self.cap.read()
            cv2.imshow("frame", frame)

            data = frame.flatten()
            self.udpsend(data)

            if cv2.waitKey(1) == ord('q'):
                break


addr = ("127.0.0.1", 6969)
if sys.argv[1] == 'opencv':
    client = OpencvClient("OpenCVClient")
else:
    client = RandomStreamClient("RandomStreamClient") 

client.connect(addr)
client.start_stream()


# sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
# sock.settimeout(1.0)
# sock.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, 300000)

# addr = ("127.0.0.1", 6969)

# cap = cv2.VideoCapture(0)

# while True:
#     ret, frame = cap.read()
#     cv2.imshow("frame", frame)

#     data = frame.flatten()

#     size = len(data)
#     segments = math.ceil(size / DGRAM)

#     start = 0
#     while segments:
#         end = min(size, start + DGRAM)
#         sock.sendto(data[start:end], addr)
        
#         start = end
#         segments -= 1

#         #res = sock.recvfrom(5)

#     if cv2.waitKey(1) == ord('q'):
#         break