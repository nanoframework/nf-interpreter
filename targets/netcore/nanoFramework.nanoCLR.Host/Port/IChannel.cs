namespace nanoFramework.nanoCLR.Host.Port
{
    public interface IChannel
    {
        IPort PortA { get; }
        IPort PortB { get; }

        public void Connect();
        public void Disconnect();
    }
}
