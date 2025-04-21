
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using System.Xml.Linq;

namespace OpenAPISampleNET
{
class Program
{
    static void Main(string[] args)
    {
        Console.WriteLine("--- Download data sample ---");
        WebClient webclient = new WebClient();
        webclient.Headers.Add("Referer", "http://www.example.com"); // Replace with your domain here
        // Registrer a handler that will execute when download is completed.
        webclient.UploadStringCompleted += (obj, arguments) =>
        {
            if (arguments.Cancelled == true)
            {
                Console.Write("Request cancelled by user");
            }
            else if (arguments.Error != null)
            {
                Console.WriteLine(arguments.Error.Message);
                Console.Write("Request Failed");
            }
            else
            {
                Console.WriteLine(formatXML(arguments.Result));
                Console.Write("Data downloaded");
            }
            Console.WriteLine(", press 'X' to exit.");
        };

        try
        {
            // API server url
            Uri address = new Uri("https://api.trafikinfo.trafikverket.se/v2/data.xml");
            string requestBody = "<REQUEST>" +
                                    "<LOGIN authenticationkey='ec3d22b6a975450c9e817bc0c7b905ea'/>" +
                                    "<QUERY objecttype='TrainStation' namespace='rail.infrastructure' schemaversion='1.5' limit='10'>" +
                                        "<FILTER>" +
                                        "</FILTER>" +
                                    "</QUERY>" +
                                "</REQUEST>";

            webclient.Headers["Content-Type"] = "text/xml";
            Console.WriteLine("Fetching data ... (press 'C' to cancel)");
            webclient.UploadStringAsync(address, "POST", requestBody);
        }
        catch (UriFormatException)
        {
            Console.WriteLine("Malformed url, press 'X' to exit.");
        }
        catch (Exception ex)
        {
            Console.WriteLine(ex.Message);
            Console.WriteLine("An error occured, press 'X' to exit.");
        }

        char keychar = Char.ToUpper(((char)Console.Read()));
        if (keychar == 'C')
        {
            webclient.CancelAsync();
        }
    }

    // Format xml so it is readable by humans.
    private static string formatXML(string xml)
    {
        // Format xml.
        XDocument rxml = XDocument.Parse(xml);
        XmlWriterSettings xmlsettings = new XmlWriterSettings();
        xmlsettings.OmitXmlDeclaration = true;
        xmlsettings.Indent = true;
        xmlsettings.IndentChars = "      ";
        var sb = new StringBuilder();
        using (XmlWriter xmlWriter = XmlWriter.Create(sb, xmlsettings))
        {
            rxml.WriteTo(xmlWriter);
        }
        return sb.ToString();
    }
}
}