from pxr import Tf
from pxr.Usdviewq.plugin import PluginContainer

class CodeProcPluginContainer(PluginContainer):

    def registerPlugins(self, plugRegistry, usdviewApi):
        dial = self.deferredImport(".dialog")
        print(__file__)
        self._launchPreferences = plugRegistry.registerCommandPlugin(
            "CodeProcPluginContainer.launchPreferences",
            "Launch Preferences",
            dial.launchDialog)

    def configureView(self, plugRegistry, plugUIBuilder):
        tutMenu = plugUIBuilder.findOrCreateMenu("CodeProc")
        tutMenu.addItem(self._launchPreferences)

Tf.Type.Define(CodeProcPluginContainer)