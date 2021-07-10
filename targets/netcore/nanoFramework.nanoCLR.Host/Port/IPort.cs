namespace nanoFramework.nanoCLR.Host.Port
{
    public interface IPort
    {
        int BytesAvailable { get; }
        byte[] ReceiveData();
        void TransmitData(byte[] data);
        void Process();
        void Open();
        void Close();
    }
}
