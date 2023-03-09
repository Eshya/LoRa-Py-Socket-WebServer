#!/usr/bin/env python3

from flask import Flask, jsonify, request
from flask_cors import CORS
from werkzeug.security import generate_password_hash, check_password_hash

app = Flask(_name_)
CORS(app)
cors = CORS(app, resource={
    r"/*":{
        "origins":"*"
    }
})
users = {
    'admin': generate_password_hash('password123'),
    'jane': generate_password_hash('123456'),
    'john': generate_password_hash('abcdef')
}

@app.route('/login', methods=['POST'])
def login():
    if not request.json or not 'username' in request.json or not 'password' in request.json:
        return jsonify({'error': 'Bad request'}), 400

    username = request.json['username']
    password = request.json['password']

    if username not in users:
        return jsonify({'error': 'Invalid username or password'}), 401

    if not check_password_hash(users[username], password):
        return jsonify({'error': 'Invalid username or password'}), 401

    return jsonify({'message': 'Login successful'}), 200

if _name_ == '_main_':
    app.run(debug=True,port=5000, host='0.0.0.0')
