using System;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Runtime.CompilerServices;
using System.Threading.Tasks;

namespace GateController
{
    public class GateAccessorData : INotifyPropertyChanged
    {
        private readonly ObservableCollection<string> _messageCollection;
        private string _ioTHubConnectionString;
        private string _deviceId;
        private string _serviceBusConnectionString;
        private string _queueName;

        public GateAccessorData(ObservableCollection<string> messageCollection)
        {
            _messageCollection = messageCollection;
            LoadFromFile();
        }

        public void LoadFromFile()
        {
            try
            {
                using (var streamWriter = File.OpenText(FilePath))
                {
                    IoTHubConnectionString = streamWriter.ReadLine();
                    DeviceId = streamWriter.ReadLine();
                    ServiceBusConnectionString = streamWriter.ReadLine();
                    QueueName = streamWriter.ReadLine();
                }
            }
            catch (Exception e)
            {
                Debug.WriteLine($"Error load settings from a file: {e.Message}");
            }
        }

        private static string FilePath => Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData), "settings.txt");


        public void SaveToFile()
        {
            try
            {
                using (var streamWriter = File.CreateText(FilePath))
                {
                    streamWriter.WriteLine(IoTHubConnectionString);
                    streamWriter.WriteLine(DeviceId);
                    streamWriter.WriteLine(ServiceBusConnectionString);
                    streamWriter.WriteLine(QueueName);
                }
            }
            catch (Exception e)
            {
                Debug.WriteLine($"Error write settings to a file: {e.Message}");
            }
        }

        public Task OnStateChangedAsync(string newState)
        {
            _messageCollection.Add(newState);
            return Task.CompletedTask;
        }

        public Task OnExceptionAsync(string exceptionMessage)
        {
            _messageCollection.Add($"Error: {exceptionMessage}");
            return Task.CompletedTask;
        }

        public string IoTHubConnectionString
        {
            get => _ioTHubConnectionString;
            set
            {
                if (_ioTHubConnectionString == value)
                    return;

                _ioTHubConnectionString = value;
                OnPropertyChanged();
            }
        }

        public string DeviceId
        {
            get => _deviceId;
            set
            {
                if (_deviceId == value)
                    return;

                _deviceId = value;
                OnPropertyChanged();
            }
        }

        public string ServiceBusConnectionString
        {
            get => _serviceBusConnectionString;
            set
            {
                if (_serviceBusConnectionString == value)
                    return;

                _serviceBusConnectionString = value;
                OnPropertyChanged();
            }
        }

        public string QueueName
        {
            get => _queueName;
            set
            {
                if (_queueName == value)
                    return;

                _queueName = value;
                OnPropertyChanged();
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;
        protected virtual void OnPropertyChanged([CallerMemberName] string propertyName = "")
        {
            PropertyChanged?.Invoke(this,
                new PropertyChangedEventArgs(propertyName));
        }

    }
}