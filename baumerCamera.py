try:
    import cBaumerCamera
except:
    strErr = "\n\n`cBaumerCamera` module not found, "
    strErr += "run `$ make car_py_module`! \n"
    raise RuntimeError(strErr)

class baumerCamera():

    def __init__(self):
        self.cameraCapsule = cBaumerCamera.construct()

    def start_camera(self):
        cBaumerCamera.StartCamera(self.cameraCapsule)

    def get_image_from_camera(self):
        cBaumerCamera.GetImageFromCamera(self.cameraCapsule)

    def stop_camera(self):
        cBaumerCamera.StopCamera(self.cameraCapsule)

    def __delete__(self):
        cBaumerCamera.delete_object(self.cameraCapsule)