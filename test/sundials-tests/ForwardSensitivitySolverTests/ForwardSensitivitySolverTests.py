import os
import sys

import numpy as np

thisDir = os.path.dirname(os.path.realpath(__file__))
rr_site_packages = os.path.dirname(os.path.dirname(thisDir))

sys.path += [
    rr_site_packages,
]
from roadrunner.roadrunner import RoadRunner, ForwardSensitivitySolver
import roadrunner.testing.TestModelFactory as tmf

import unittest


class FFSTests(unittest.TestCase):
    maxDiff = None

    def setUp(self) -> None:

        self.testModel = tmf.TestModelFactory("OpenLinearFlux")
        self.rr = RoadRunner(self.testModel.str())
        self.model = self.rr.getModel()
        print(self.model)
        self.sens = ForwardSensitivitySolver(self.model)

    def checkMatricesEqual(self, expected, actual, places=7):
        for i in range(expected.shape[0]):
            for j in range(expected.shape[1]):
                self.assertAlmostEqual(expected[i, j], actual[i, j], places=places)

    def checkSensWithModel(self, modelName: str, places: int = 7):
        """Check that we can integrate a model in Python without the RoadRunner object

        Args:
            modelName: The name of a test model to integrate. The test model must implement th e
                        timeSeriesResult interface. See TestModelFactor.h
            places: number of decimal places to use for floating point comparisons
        """
        if modelName not in tmf.getAvailableTestModels():
            raise ValueError(f"Model name \"{modelName}\ not available. Here are a list of "
                             f"available model names: {tmf.getAvailableTestModels()}")
        testModel = tmf.TestModelFactory(modelName)
        if tmf.TimeSeriesSensitivityResult not in testModel.__class__.mro():
            raise ValueError("This TestModel does not implement the TimeSeriesSensitivityResult interface. ")

        # we need a RoadRunner object to create a model for us
        rr = RoadRunner(testModel.str())
        model = rr.getModel()
        integrator = ForwardSensitivitySolver(model)

        # all tests are done with stiff set to False
        # this is probably a weakness
        integrator.setValue("stiff", False)

        settingsMap = testModel.timeSeriesSensitivityResultSettings()
        start = settingsMap["start"]
        duration = settingsMap["duration"]
        steps = settingsMap["steps"]

        # always have one more number of data points (in the row direction) than there are number of steps / intervals
        num = steps + 1
        stepSize = (duration - start) / steps

        # collect true values from the TestModel type
        expectedResults = testModel.timeSeriesSensitivityResult()

        print(expectedResults)
        #
        # # a place to store actual test result data
        # results = np.zeros(expectedResults.shape)
        #
        # # sensitivities for t=0 are always 0
        # results[0, 0] = rr.getCurrentTime()
        # numStates = len(rr.getFloatingSpeciesInitialConcentrationIds())
        #
        # for j in range(1, numStates + 1):
        #     results[0, j] = 0
        #
        # t = start
        # for i in range(1, num):
        #     t = integrator.integrate(t, stepSize)
        #     results[i, 0] = rr.getCurrentTime()
        #
        #     # for j in range(1, numStates + 1):
        #     #     results[i, j] = rr.getFloatingSpeciesAmountsNamedArray()[0][j - 1]
        #
        # print(expectedResults)
        # print(results)
        # self.checkMatricesEqual(expectedResults, results, places=places)
        #

    def testOpenLinearFluxWithoutRoadRunnerInstance(self):
        self.checkSensWithModel("OpenLinearFlux", 5)

    def test_getGlobalParameterNames(self):
        actual = self.sens.getGlobalParameterNames()
        expected = ('kin', 'kf', 'kout', 'kb')
        self.assertEqual(expected, actual)

    def test_getVariableNames(self):
        actual = self.sens.getVariableNames()
        expected = ('S1', 'S2')
        self.assertEqual(expected, actual)

    def test_CheckValueOfNp(self):
        actual = self.sens.Np
        expected = 4
        self.assertEqual(expected, actual)

    def test_CheckValueOfNsWhenDefaultToAllParameters(self):
        actual = self.sens.Ns
        expected = 4
        self.assertEqual(expected, actual)

    def test_CheckValueOfNsWhenSelectingParameters(self):
        sens2 = ForwardSensitivitySolver(self.model, ["kf"])
        actual = sens2.Ns
        expected = 1
        self.assertEqual(expected, actual)

    def test_CheckValueOfNumModelVariables(self):
        actual = self.sens.numModelVariables
        expected = 2
        self.assertEqual(expected, actual)

    def test_getModelParametersAsMap(self):
        actual = self.sens.getModelParametersAsMap()
        expected = {'kb': 0.01, 'kf': 0.1, 'kin': 1.0, 'kout': 0.2}
        self.assertEqual(expected, actual)

    def test_getModelParametersAsVector(self):
        actual = self.sens.getModelParametersAsVector()
        expected = np.array([1., 0.1, 0.2, 0.01])
        self.assertTrue((expected == actual).all())

    def test_deducePlistDefaultToAllParameters(self):
        self.sens.deducePlist()
        actual = self.sens.plist
        expected = (0, 1, 2, 3)
        self.assertEqual(expected, actual)

    def test_deducePlistFirstParameter(self):
        sens2 = ForwardSensitivitySolver(self.model, ["kin"])
        sens2.deducePlist()
        actual = sens2.plist
        expected = (0,)
        self.assertEqual(expected, actual)

    def test_deducePlistSecondParameter(self):
        sens2 = ForwardSensitivitySolver(self.model, ["kf"])
        sens2.deducePlist()
        actual = sens2.plist
        expected = (1,)
        self.assertEqual(expected, actual)

    def test_SettingsNonLinearSolver(self):
        self.sens.setValue("nonlinear_solver", "fixed_point")
        actual = self.sens.getValue("nonlinear_solver")
        expected = "fixed_point"
        self.assertEqual(expected, actual)
