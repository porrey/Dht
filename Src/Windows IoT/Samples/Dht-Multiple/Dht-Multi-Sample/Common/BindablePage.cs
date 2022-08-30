using System.Collections.Generic;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using Windows.UI.Xaml.Controls;

namespace Dht.Multi.Sample.Common
{
	public abstract class BindablePage : Page, INotifyPropertyChanged
	{
		public event PropertyChangedEventHandler PropertyChanged;

		protected virtual bool SetProperty<T>(ref T storage, T value, [CallerMemberName] string propertyName = null)
		{
			if (EqualityComparer<T>.Default.Equals(storage, value)) return false;

			storage = value;
			this.RaisePropertyChanged(propertyName);

			return true;
		}

		protected void RaisePropertyChanged([CallerMemberName]string propertyName = null)
		{
			this.OnPropertyChanged(propertyName);
		}

		protected virtual void OnPropertyChanged([CallerMemberName]string propertyName = null)
		{
			OnPropertyChanged(new PropertyChangedEventArgs(propertyName));
		}

		protected virtual void OnPropertyChanged(PropertyChangedEventArgs args)
		{
			this.PropertyChanged?.Invoke(this, args);
		}
	}
}
