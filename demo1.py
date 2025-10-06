# Python program to translate
# speech to text and text to speech


import speech_recognition as sr
import pyttsx3 
import requests

# Initialize the recognizer 
r = sr.Recognizer() 

# Function to convert text to speech
def SpeakText(command):
    
    # Initialize the engine
    engine = pyttsx3.init(driverName='nsss')
    voices = engine.getProperty('voices')                     
    engine.say(command) 
    engine.runAndWait()
    
    
# Loop infinitely for user to speak
while(1):    
    
    # Exception handling to handle
    # exceptions at the runtime
    try:
        
        # use the microphone as source for input.
        with sr.Microphone() as source2:
            
            # wait for a second to let the recognizer adjust the energy threshold based on the surrounding noise level 
            r.adjust_for_ambient_noise(source2, duration=0.2)
            
            print("Actively listening. Say something.")
            # listens for the user's input 
            audio2 = r.listen(source2)
            # audio2 = open("filename.wav")
            
            # Using google to recognize audio
            # MyText = r.recognize_google(audio2)
            # or use whisper here:
            MyText = r.recognize_whisper(audio2, language="english")
            MyText = MyText.lower()
            # MyText = "Write a poem on sunrise." # adjustment for bigbend

            # making llama-server POST request
            url = "http://127.0.0.1:8080/completion"
            data = {
                "prompt": MyText,
                "n_predict": 64
            }

            print("Sending to LLM: ",data)
            result = requests.post(url, json=data)

            print("Did you say",result.json()['content'])
            SpeakText(result.json()['content'])
            
    except sr.RequestError as e:
        print("Could not request results; {0}".format(e))
        
    except sr.UnknownValueError:
        print("unknown error occurred")
