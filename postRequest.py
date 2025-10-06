import requests
url = "http://172.22.70.20:8888/completion"
prompt = "Write a haiku on sunset"
data = {
    "prompt": prompt, 
    "n_predict": 128,
    "temperature": 0.4,
    "stream": True
}

print("Sending to LLM: ",data)
result = requests.post(url, json=data)

# print("Response: ",result.json()['content'])
# for stream response
print("Response: ", result.headers)