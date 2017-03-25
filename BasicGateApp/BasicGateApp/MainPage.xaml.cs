using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Microsoft.Azure.Devices.Client;
using Xamarin.Forms;
using Message = Microsoft.Azure.Devices.Client.Message;

namespace BasicGateApp
{
	public partial class MainPage : ContentPage
	{
		public MainPage()
		{
			InitializeComponent();
		}

	    public async void OnActivate(object sender, EventArgs args)
	    {
	        try
	        {
                string iotHubConnectionString = "[paste IoT hub device connection string here]";
                var client = DeviceClient.CreateFromConnectionString(iotHubConnectionString);
                await client.SendEventAsync(new Message(Encoding.ASCII.GetBytes("Activate")));
            }
	        catch (Exception e)
	        {
                await DisplayAlert("Exception", e.Message, "OK");
            }
	    }
	}
}
