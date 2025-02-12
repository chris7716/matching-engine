import socket
import random
import time
import uuid

# Server details
HOST = '127.0.0.1'  # Change if needed
PORT = 8080
OUTPUT_FILE = "trades.log"

# Order types
ORDER_TYPES = ['Buy', 'Sell']

def generate_order(matchable=False):
    """Generate a random order. If matchable is True, ensure the order can be matched."""
    order_id = str(uuid.uuid4())
    order_type = random.choice(ORDER_TYPES)
    price = round(random.uniform(50, 200), 2)
    quantity = random.randint(1, 100)

    if matchable:
        # To ensure the order is matchable, create a buy order with a price >= sell order
        # For simplicity, assume the "sell" price is always generated within a range
        if order_type == 'Buy':
            # For "Buy", ensure it has a price greater than or equal to a sell order
            price = round(random.uniform(50, 200), 2)
        else:
            # For "Sell", set a price within a known range to make it matchable
            price = round(random.uniform(50, 150), 2)

    return f"{order_id},{order_type},{price},{quantity}"

def send_orders(rate=10):
    """Send random orders to the matching engine and log responses."""
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s, open(OUTPUT_FILE, "w") as log_file:
        s.connect((HOST, PORT))
        print(f"Connected to {HOST}:{PORT}")

        while True:
            # Decide whether to send a matchable order or a random order
            matchable = random.random() < 0.2  # 20% chance to send matchable orders
            order = generate_order(matchable=matchable)
            s.sendall(order.encode() + b'\n')
            print(f"Sent: {order}")

            # Receive and log responses
            response = s.recv(1024).decode().strip()
            if response:
                log_file.write(response + "\n")
                log_file.flush()
                print(f"Logged: {response}")

            time.sleep(1 / rate)  # Control the order generation speed

if __name__ == "__main__":
    send_orders(rate=50)  # Adjust rate as needed
