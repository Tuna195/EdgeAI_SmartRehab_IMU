import socket
import csv

UDP_IP = "0.0.0.0" # Lắng nghe tất cả các card mạng
UDP_PORT = 12345

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

print("Đang chờ dữ liệu từ ESP32...")
filename = "bicep_curl_raw.csv"

with open(filename, "w", newline="") as f:
    writer = csv.writer(f)
    writer.writerow(["timestamp", "ax", "ay", "az", "gx", "gy", "gz"])
    try:
        while True:
            data, addr = sock.recvfrom(1024)
            row = data.decode().split(",")
            writer.writerow(row)
            print(row)
    except KeyboardInterrupt:
        print("Đã dừng và lưu file.")