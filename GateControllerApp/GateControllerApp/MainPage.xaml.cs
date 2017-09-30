using System;
using System.Net.Http;
using System.Net.Http.Headers;
using System.Threading.Tasks;
using Xamarin.Forms;

namespace GateControllerApp
{
    public partial class MainPage : ContentPage
    {
        private readonly GateMessages _gateMessages = new GateMessages();

        public MainPage()
        {
            InitializeComponent();
            GateMessagesView.ItemsSource = _gateMessages.Messages;
            
            InitGetDeviceStatusAsync();

        }

        private async void InitGetDeviceStatusAsync()
        {
            while (true)
            {
                
                try
                {
                    var result = await GetGateStatusMessage("telemetryqueue");
                    result = result.Trim('"');
                    if (!result.ToLower().StartsWith("error"))
                    {
                        _gateMessages.Messages.Add(result);
                    }
                    else
                    {
                        await Task.Delay(TimeSpan.FromSeconds(15));
                    }
                }
                catch (Exception e)
                {
                    await DisplayAlert("Error", e.Message, "OK");
                }
                
            }
        }

        async Task<string> GetGateStatusMessage(string queueName)
        {
            using (var client = new HttpClient())
            {
                string result;


                client.DefaultRequestHeaders.Accept.Clear();
                client.DefaultRequestHeaders.Accept.Add(new MediaTypeWithQualityHeaderValue("text/html"));
                var uri =
                    $"https://iothubgateproxy.azurewebsites.net/api/GetGateStatusMessages?code=J3hhy1Cq2imacpSnmkh5/Fkhau4g5ibRu1cBeLxLU2qkPoWXUnbGjQ==&statusQueue={queueName}";
                HttpResponseMessage response = await client.GetAsync(uri);

                result = await response.Content.ReadAsStringAsync();

                return result;
            }
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
                        $"https://iothubgateproxy.azurewebsites.net/api/SendCommandToGate?code=l1dr7kawEXd2XOWrUNO9bYtzftPViqFB3Jnxy1rXUFfAg7JjhfZQSA==&device={deviceId}&command={command}";
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

        private async Task OnActivateModelGateAsync(string command)
        {
            var result = await SendCloudToDeviceCommandAsync("Gate", command);
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
            var result = await DisplayAlert("Reset to web",
                "Are you sure you want to turn off Azure IoT Hub communication?", "Yes", "No");
            if (result)
            {
                await OnActivateModelGateAsync("Web");
            }
        }
    }
}
