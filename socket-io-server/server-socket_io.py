#!/usr/bin/env python3

import eventlet
import socketio
from flask import Flask
from flask_cors import CORS

# Create a Flask app instance
app = Flask(__name__)
CORS(app)

# Create a Socket.IO server instance
sio = socketio.Server(cors_allowed_origins='*')

# Define a Socket.IO event handler for the "connect" event
@sio.on('connect')
def on_connect(sid, environ):
    print(f"Client connected: {sid}")
    
@sio.on('register')
def on_register(sid, data):
    client_id = data.get('client_id')
    sio.enter_room(sid, client_id)
    print(f"Client with ID {client_id} registered.")
# Define a Socket.IO event handler for the "disconnect" event
@sio.on('disconnect')
def on_disconnect(sid):
    print(f"Client disconnected: {sid}")
    

# Define a Socket.IO event handler for a custom "message" event
@sio.on('message')
def on_message(sid, data):
    print(f"Received message from client {sid}: {data}")
    sio.emit('message', data, room=sid) # Send the message back to the client that sent it

# Define a Socket.IO event handler for a custom "find_client" event
@sio.on('find_client')
def on_find_client(sid, client_id):
    print(f"Received request to find client {client_id}")
    found = False

    for room_sid in sio.rooms(sid):
        if client_id in sio.rooms(room_sid):
            sio.emit('client_found', {'client_id': client_id, 'room_sid': room_sid}, room=sid)
            found = True
            break
    if not found:
        sio.emit('client_not_found', {'client_id': client_id}, room=sid) # Add "room=sid" argument here

# Run the Socket.IO server
if __name__ == '__main__':
    app = socketio.WSGIApp(sio, app)
    eventlet.wsgi.server(eventlet.listen(('localhost', 8000)), app)
