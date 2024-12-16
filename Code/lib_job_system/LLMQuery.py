import ollama
from ollama import chat
from ollama import ChatResponse
import json
import sys

print("\nThis is CL argv: ", sys.argv, "\n")

inFilePath = ''.join(sys.argv[1])
outFilePath = ''.join(sys.argv[2])

with open(inFilePath, "r") as infile:
    lines = infile.readlines()

prompt = '\n'.join(lines)

print("\nThis is INFILE: ", inFilePath, "\n")
print("\nThis is OUTFILE: ", outFilePath, "\n")
print("\nThis is PROMPT: ", prompt, "\n")

model = 'llama3.2:1b'

try:
  ollama.chat(model)
except ollama.ResponseError as e:
  print('Error:', e.error)
  if e.status_code == 404:
    ollama.pull(model)

# Make the API request
# response = openai.Completion.create(
#     model=model,
#     prompt=prompt,
#     max_tokens=5000,
#     temperature=0.28,
#     top_p=0.95,
#     n=1,
#     echo=True,
#     stream=False
# ) # Deprecated

response = chat(model=model, messages=[
  {
    'role': 'user',
    'content': prompt,
    "options": {
        "temperature": 0
    },
    "stream": False,
  },
])
# print(response['message']['content']) # or access fields directly from the response object response.message.content

response = json.loads(response.json())

# Print the generated completion
print("\nResponse: ",response)
print(type(response))

json_str = json.dumps(response, indent=4)

with open(outFilePath, "w") as outfile:
    json.dump(response, outfile)
