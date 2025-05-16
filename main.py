import torch
import cv2
torch.set_num_threads(1)
cv2.setNumThreads(1)

import argparse
import time
from ultralytics import YOLO
import concurrent.futures
import numpy as np

class PoseModel:
    def __init__(self):
        self.model = YOLO('yolov8s-pose.pt')
    
    def custom_predict(self, frame):
        results = self.model(frame, verbose=False)
        return results[0].plot()

def read_frames(input_path):
    cap = cv2.VideoCapture(input_path)
    frames = []
    while True:
        ret, frame = cap.read()
        if not ret:
            break
        frames.append(frame)
    cap.release()
    return frames

def write_video(output_path, frames, fps):
    if not frames:
        return
    height, width = frames[0].shape[:2]
    
    fourcc = cv2.VideoWriter_fourcc(*'mp4v')  
    out = cv2.VideoWriter(output_path, fourcc, fps, (width, height))
    
    for frame in frames:
        out.write(frame)

    out.release()
    print(f"The video has been successfully saved: {output_path}")

def process_single_thread(input_path):
    model = PoseModel()
    frames = read_frames(input_path)
    processed_frames = []

    start_time = time.time()
    for frame in frames:
        processed_frames.append(model.predict(frame))
    print(f"Processing time: {time.time() - start_time:.2f} seconds")
    write_video("output_single.mp4", processed_frames, 30)

def init_process():
    global model
    model = PoseModel()

def process_frame(frame):
    return model.custom_predict(frame)

def process_multi_thread(input_path, workers):
    frames = read_frames(input_path)
    start_time = time.time()
    with concurrent.futures.ProcessPoolExecutor(max_workers=workers, initializer=init_process) as executor:
        processed_frames = list(executor.map(process_frame, frames))
    print(f"Processing time: {time.time() - start_time:.2f} seconds")
    write_video("output_multi.avi", processed_frames, 30)

def main():
    parser = argparse.ArgumentParser(description='Video processing with YOLOv8.')
    parser.add_argument('--input', type=str, required=True)
    parser.add_argument('--mode', choices=['single', 'multi'], required=True)
    parser.add_argument('--workers', type=int, default=4)
    args = parser.parse_args()

    if args.mode == 'single':
        process_single_thread(args.input)
    else:
        process_multi_thread(args.input, args.workers)

if __name__ == "__main__":
    main()