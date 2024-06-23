from audiocraft.models import MusicGen
from audiocraft.data.audio import audio_write
from datetime import datetime
import json
import os
import re


def load_config_from_json(filename="config.json"):
    config=set_default_values()
    try:
        with open(os.path.join(os.path.dirname(__file__), filename), "r") as cfg_file:
            loaded_config = json.load(cfg_file)
            config.update(loaded_config)
    except (FileNotFoundError, ValueError, KeyError):
        pass

    print("\nConfiguration:")
    print("-" * 22)
    for key, val in sorted(config.items(), key=lambda item: item[0].upper()):
        print(key.capitalize().replace("_", " ").strip() + ": " + str(val))
    print()
    
    return config["BPM"], config["timeSigNumerator"], config["timeSigDenominator"], config["numberOfGens"],config["scale"], config["prompt"]

def set_default_values():
    return {
        "BPM": 140,
        "timeSigNumerator": 4,
        "timeSigDenominator": 4,
        "numberOfGens": 1,
        "scale": "C Minor",
        "prompt": "random beautiful melody"
    }

if __name__ == "__main__":
    goOnEditing = True
    bpm, timeSigNumerator, timeSigDenominator, numberOfGens,scale, prompt = load_config_from_json()
    durationFromBpm = timeSigNumerator * timeSigDenominator * 60 / bpm
    description = f"Produce a 4-bar melodic sequence in the key of {scale}, {bpm} BPM, {timeSigNumerator}/{timeSigDenominator} time signature, {prompt}. Exclude drums and percussion, Focus solely on the melody, without any accompaniment or rhythm section. "
    descriptions=[description]
    if numberOfGens >1:
        for x in range(numberOfGens-1):
            descriptions.append(description)

    ##Launching model
    ## facebook/musicgen-large
    ## facebook/magnet-medium-10secs
    ## facebook/musicgen-stereo-melody-large
    modelName = "facebook/musicgen-stereo-melody-large"
    model = MusicGen.get_pretrained(modelName)
    model.set_generation_params(duration=durationFromBpm) 
    wav = model.generate(descriptions, progress=True) 

    dateTime= datetime.strftime(datetime.utcnow(), '%Y%m%d_%H%M%S')
    outPutFileName = re.sub(r'[^a-zA-Z0-9\s]', '', prompt +'' + dateTime).replace(" ", "_")

    for idx, one_wav in enumerate(wav):
         output_path = f"{prompt}/{outPutFileName}{idx}"
         print(f"Saving audio to: {output_path}")
        # Save audio file with loudness normalization at -14 db LUFS.
         audio_write(output_path, one_wav.cpu(), model.sample_rate, strategy="loudness")
    exit()