using System;
using System.Text;
using System.Threading.Tasks;
using Microsoft.Azure.Devices;

namespace GateAccessor
{
    
    

    public class GateAccessor
    {
        
        private static readonly string _connectionString = "{iot hub connection string}";
        private static readonly string _deviceId = "Gate";


        public static async Task SendCommand(GateCommand command)
        {
            var serviceClient = ServiceClient.CreateFromConnectionString(_connectionString);
            var commandMessage = new Message(Encoding.ASCII.GetBytes(command.ToString()));
            await serviceClient.SendAsync(_deviceId, commandMessage);
        }
    }

    public enum GateCommand
    {
        Open,
        Close,
        Stop,
        Web
    }
}
