# hl2av

Simple H264 Encoder/Decoder for HoloLens 2 using Media Foundation.

## Unity Sample

A sample Unity project (2020.3.42f1) can be found in the [hl2av_unity](hl2av_unity) directory.
See the [IPCSkeleton.cs](hl2av_unity/Assets/Scripts/IPCSkeleton.cs) script for an example of using the hl2av plugin in Unity.
The example works as follows:
1. The Unity App uses [hl2ss](https://github.com/jdibenes/hl2ss) to stream video from the HoloLens RGB camera over WiFi.
2. The Python script [viewer/test_hl2av_2.py](viewer/test_hl2av_2.py) receives the video stream, uses PyAV to re-encode the video, and sends it back to the Unity App running on the HoloLens.
3. The Unity App uses the hl2av plugin to decode the stream, then re-encodes it and sends it back to the Python script running on the PC.
4. The Python script uses PyAV to decode the video and displays it using OpenCV.

**Build and run the sample project**

1. Open the project in Unity. If the MRTK Project Configurator window pops up just close it.
2. Go to Build Settings (File -> Build Settings).
3. Switch to Universal Windows Platform.
4. Set Target Device to HoloLens.
5. Set Architecture to ARM64.
6. Set Build and Run on Remote Device (via Device Portal).
7. Set Device Portal Address to your HoloLens IP address (e.g., https://192.168.1.7) and set your Device Portal Username and Password.
8. Click Build and Run. Unity may ask for a Build folder. You can create a new one named Build.

## References

This project is part of the [hl2ss](https://github.com/jdibenes/hl2ss) project.
