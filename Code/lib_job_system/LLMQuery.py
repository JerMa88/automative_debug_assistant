import openai
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

openai.api_base = "http://localhost:4891/v1"
#openai.api_base = "https://api.openai.com/v1"

openai.api_key = "not needed for a local LLM"

# Set up the prompt and other parameters for the API request
# prompt = "Related to your given file path, debug my code."

# model = "gpt-3.5-turbo"
# model = "mpt-7b-chat"
model = "mistral-7b-openorca.Q4_0.gguf"
# model = "gpt4all-falcon-q4_0.gguf"

# Make the API request
response = openai.Completion.create(
    model=model,
    prompt=prompt,
    max_tokens=5000,
    temperature=0.28,
    top_p=0.95,
    n=1,
    echo=True,
    stream=False
)

# Print the generated completion
print("\nThis is responce: \'",response, "\'")

# json_str = json.dumps(response, indent=4)

with open(outFilePath, "w") as outfile:
    json.dump(response, outfile)
