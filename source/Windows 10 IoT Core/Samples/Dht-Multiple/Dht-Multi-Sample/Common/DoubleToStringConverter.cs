using System;
using Windows.UI.Xaml.Data;

namespace Dht.Multi.Sample.Common
{
	public sealed class DoubleToStringConverter : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, string language)
		{
			return $"{value:0.0}";
		}

		public object ConvertBack(object value, Type targetType, object parameter, string language)
		{
			return System.Convert.ToDouble(value);
		}
	}
}