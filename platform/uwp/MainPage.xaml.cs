using Windows.ApplicationModel.Core;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace Prueba3D
{
    public sealed partial class MainPage : Page
    {
        public MainPage()
        {
            this.InitializeComponent();

            // Extiende la vista de tu UI en la barra de título
            var coreTitleBar = CoreApplication.GetCurrentView().TitleBar;
            coreTitleBar.ExtendViewIntoTitleBar = true;

            // Botones nativos siguen visibles, fondo transparente
            coreTitleBar.ButtonBackgroundColor = Colors.Transparent;
            coreTitleBar.ButtonInactiveBackgroundColor = Colors.Transparent;

            // Le indicamos al sistema cuál es la "barra de título"
            Window.Current.SetTitleBar(TitleBarArea);
        }
    }
}