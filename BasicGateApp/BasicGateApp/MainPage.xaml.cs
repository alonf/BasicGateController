﻿using System;
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
		public MainPage()
		{
			InitializeComponent();
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

	    private async void OnToggleBlueLed(object sender, EventArgs e)
	    {
	        var result = await SendCloudToDeviceCommandAsync("SimpleRelay", "Toggle");
	        await DisplayAlert("Fun Propellor HTTP Get result", result, "OK");
	    }

	    private  async Task OnActivateMiniatureGateAsync(string command)
	    {
            var result = await SendCloudToDeviceCommandAsync("MiniatureGate", command);
            await DisplayAlert("Miniature Gate HTTP Get result", result, "OK");
        }

	    private async void OnStopMiniatureGate(object sender, EventArgs e)
	    {
	        await OnActivateMiniatureGateAsync("Stop");
	    }

	    private async void OnOpenMiniatureGate(object sender, EventArgs e)
	    {
            await OnActivateMiniatureGateAsync("Open");
        }

	    private async void OnCloseMiniatureGate(object sender, EventArgs e)
	    {
            await OnActivateMiniatureGateAsync("Close");
        }
	}
}
