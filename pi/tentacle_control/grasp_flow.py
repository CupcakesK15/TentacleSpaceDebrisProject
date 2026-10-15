from .enums import TentacleStatus


class GraspFlow:
    def __init__(self, manager, classifier):
        self.manager = manager
        self.classifier = classifier

    def perform_grasp(self):
        # 1. detect object
        obj = self.detect_object()

        # 2. align tentacles
        self.manager.align_to_object(obj)

        # 3. curl tentacles (returns immediately; the Picos move on their own)
        self.manager.curl_all(0.3)

        # 3b. wait for every tentacle to finish, or give up and let go
        statuses = self.manager.wait_all()
        if any(s != TentacleStatus.DONE for s in statuses):
            self.manager.retract_all()
            return "GRASP_FAILED"

        # 4. verify grasp
        if not self.manager.verify_grasp():
            self.manager.retract_all()
            return "GRASP_FAILED"

        # 5. run spectrometer
        spectrum = self.classifier.capture()

        # 6. classify material
        material = self.classifier.predict(spectrum)

        return material

    def detect_object(self):
        pass
