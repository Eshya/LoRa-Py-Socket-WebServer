import requests

url = 'http://localhost:5000/login'
payload = {'username': 'admin', 'password': 'password12'}
headers = {'Content-Type': 'application/json'}

response = requests.post(url, json=payload, headers=headers)
print(response.status_code)
print(response.json())
