using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace GateController
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class SettingsPage : ContentPage
    {
        private readonly GateAccessorData _gateAccessorData;

        public SettingsPage(GateAccessorData gateAccessorData)
        {
            _gateAccessorData = gateAccessorData;
            BindingContext = _gateAccessorData;
            InitializeComponent();
        }

        private async void OnOk(object sender, EventArgs e)
        {
            _gateAccessorData.SaveToFile();
            await Navigation.PopModalAsync();
        }

        private async void OnCancel(object sender, EventArgs e)
        {
            _gateAccessorData.LoadFromFile();
            await Navigation.PopModalAsync();
        }
    }
}