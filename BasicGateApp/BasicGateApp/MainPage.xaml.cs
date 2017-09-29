using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Http;
using System.Net.Http.Headers;
using System.Text;
using System.Threading.Tasks;
using Xamarin.Forms;

namespace BasicGateApp
{
	public partial class MainPage : ContentPage
	{
	    private GateMessages _gateMessages = new GateMessages();

		public MainPage()
		{
		    InitializeComponent();
		    GateMessagesView.ItemsSource = _gateMessages.Messages;

        }

	    async Task<string> SendCloudToDeviceCommandAsync(string deviceId, string command)
	    {
            using (var client = new HttpClient())
            {
                string result;
                
                try
                {
                   client.DefaultRequestHeaders.Accept.Clear();
                   client.DefaultRequestHeaders.Accept.Add(new MediaTypeWithQualityHeaderValue("text/html"));
                    var uri =
                        $"https://iothubsupport.azurewebsites.net/api/SendCloudToDeviceCommand?code=9RIGJq8AAC6P4F9x/D0U4fpnouqwO3h7OGrFoYxfjHXQSJaF4ZcTpA==&device={deviceId}&command={command}";
                    HttpResponseMessage response = await client.GetAsync(uri);

                    result = await response.Content.ReadAsStringAsync();
                    result = $"{response.StatusCode} : {result}";
                }
                catch (Exception exception)
                {
                    result = exception.Message;
                }
                return result;
            }
        }

	    private  async Task OnActivateModelGateAsync(string command)
	    {
            var result = await SendCloudToDeviceCommandAsync("ModelGate", command);
            await DisplayAlert("Model Gate HTTP Get result", result, "OK");
        }

	    private async void OnStopModelGate(object sender, EventArgs e)
	    {
	        await OnActivateModelGateAsync("Stop");
	    }

	    private async void OnOpenModelGate(object sender, EventArgs e)
	    {
            await OnActivateModelGateAsync("Open");
        }

	    private async void OnCloseModelGate(object sender, EventArgs e)
	    {
            await OnActivateModelGateAsync("Close");
        }

        private async void OnSwitchToWebModelGate(object sender, EventArgs e)
	    {
            await OnActivateModelGateAsync("Web");
        }
	}
}
