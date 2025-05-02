import cv2
import queue
import threading
import time
import logging
import argparse
from datetime import datetime
import os
import numpy as np

if not os.path.exists('log'):
    os.makedirs('log')
logging.basicConfig(
    filename=f'log/{datetime.now().strftime("%Y-%m-%d_%H-%M-%S")}.log',
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)

class Sensor:
    def get(self):
        raise NotImplementedError("Subclass must implement method get()")

class SensorX(Sensor):
    def __init__(self, delay: float):
        self._delay = delay
        self._data = 0
    
    def get(self) -> int:
        time.sleep(self._delay)
        self._data += 1
        return self._data

class SensorCam(Sensor):
    def __init__(self, cam_name, resolution):
        self.cam_name = cam_name
        self.width, self.height = map(int, resolution.split('x'))
        self.cap = cv2.VideoCapture(self.cam_name)
        
        if not self.cap.isOpened():
            logging.error(f"Failed to open camera: {self.cam_name}")
            raise RuntimeError(f"Failed to open camera: {self.cam_name}")
        
        self.cap.set(cv2.CAP_PROP_FRAME_WIDTH, self.width)
        self.cap.set(cv2.CAP_PROP_FRAME_HEIGHT, self.height)
        logging.info(f"Camera {self.cam_name} initialized with resolution {resolution}")

    def get(self):
        ret, frame = self.cap.read()
        if not ret:
            logging.error("Failed to grab frame from camera")
            return None
        return frame

    def __del__(self):
        if hasattr(self, 'cap') and self.cap.isOpened():
            self.cap.release()
        logging.info("Camera resources released")

class SensorThread(threading.Thread):
    def __init__(self, sensor: Sensor, queue: queue.Queue):
        super().__init__()
        self.sensor = sensor
        self.queue = queue
        self._stop_event = threading.Event()
        self.daemon = True

    def run(self):
        while not self._stop_event.is_set():
            try:
                data = self.sensor.get()
                if data is not None:
                    if self.queue.full():
                        self.queue.get_nowait()
                    self.queue.put_nowait(data)
            except Exception as e:
                logging.error(f"SensorThread error: {str(e)}")
                time.sleep(0.1)

    def stop(self):
        self._stop_event.set()
        self.join()

class WindowImage:
    def __init__(self, display_fps: float):
        self.window_name = "Sensor Display"
        self.display_interval = 1.0 / display_fps
        cv2.namedWindow(self.window_name, cv2.WINDOW_NORMAL)
        self.sensor_data = {}
        logging.info(f"Window initialized with {display_fps} FPS")

    def update_display(self, camera_frame, sensor_values: dict):
        if camera_frame is None:
            return

        height, width = camera_frame.shape[:2]
        info_panel = np.full((height, width // 2, 3), 255, dtype=np.uint8)
        
        for i, (name, value) in enumerate(sensor_values.items()):
            text = f"{name}: {value}" if value is not None else f"{name}: No data"
            cv2.putText(info_panel, text, (10, 30 + i*30), 
                       cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 0), 1)
        
        combined = np.hstack((camera_frame, info_panel))
        cv2.imshow(self.window_name, combined)

    def should_close(self) -> bool:
        return cv2.waitKey(1) & 0xFF == ord('q')

    def close(self):
        if hasattr(self, 'window_name'):
            cv2.destroyWindow(self.window_name)
            logging.info("Window resources released")

    def __del__(self):
        self.close()

def main():
    parser = argparse.ArgumentParser(description="Sensor and Camera Data Display")
    parser.add_argument("--camera", default="/dev/video0", help="Camera device name") #Get-PnpDevice -Class Camera
    parser.add_argument("--resolution", default="640x480", help="Camera resolution")
    parser.add_argument("--fps", type=float, default=30.0, help="Display frequency")
    args = parser.parse_args()

    threads = []
    window = None
    camera = None
    sensors = {}

    try:
        camera = SensorCam(args.camera, args.resolution)
        sensors = {
            "Sensor0 (100Hz)": SensorX(0.01),
            "Sensor1 (10Hz)": SensorX(0.1),   
            "Sensor2 (1Hz)": SensorX(1)   
        }

        camera_queue = queue.Queue(maxsize=1)
        sensor_queues = {name: queue.Queue(maxsize=1) for name in sensors}

        threads = [
            SensorThread(camera, camera_queue),
            *[SensorThread(sensor, sensor_queues[name]) for name, sensor in sensors.items()]
        ]

        for thread in threads:
            thread.start()


        window = WindowImage(args.fps)
        last_sensor_values = {name: None for name in sensors}

        while not window.should_close():
            start_time = time.time()

            try:
                camera_frame = camera_queue.get_nowait()
            except queue.Empty:
                camera_frame = None

            for name in sensors:
                try:
                    last_sensor_values[name] = sensor_queues[name].get_nowait()
                except queue.Empty:
                    pass

            window.update_display(camera_frame, last_sensor_values)

            elapsed = time.time() - start_time
            sleep_time = max(0, 1.0/args.fps - elapsed)
            time.sleep(sleep_time)

    except Exception as e:
        logging.error(f"Error in main: {str(e)}", exc_info=True)
    finally:
        try:
            if window is not None:
                window.close()
        except Exception as e:
            logging.error(f"Error closing window: {str(e)}")

        try:
            for thread in threads:
                if hasattr(thread, 'stop'):
                    thread.stop()
        except Exception as e:
            logging.error(f"Error stopping threads: {str(e)}")

        try:
            if camera is not None:
                del camera
        except Exception as e:
            logging.error(f"Error releasing camera: {str(e)}")

        logging.info("Program terminated")

if __name__ == "__main__":
    main()