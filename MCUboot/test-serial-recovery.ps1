# MCUboot serial recovery - image list probe
# Change COM port to match your device (check Device Manager)
$PORT = "COM8"
$BAUD = 115200   # irrelevant for USB CDC but required by the API

function Get-Crc16Ccitt([byte[]]$data) {
    $crc = 0
    foreach ($b in $data) {
        $crc = $crc -bxor ([int]$b -shl 8)
        for ($i = 0; $i -lt 8; $i++) {
            if ($crc -band 0x8000) { $crc = (($crc -shl 1) -bxor 0x1021) -band 0xFFFF }
            else                   { $crc = ($crc -shl 1) -band 0xFFFF }
        }
    }
    return $crc
}

# SMP header: GET image list (op=0, flags=0, len=1, group=1, seq=0, id=0) + CBOR {}
$smp   = [byte[]](0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0xA0)
$crc    = Get-Crc16Ccitt $smp
$crcHi  = [byte](($crc -shr 8) -band 0xFF)
$crcLo  = [byte]($crc -band 0xFF)
$smpCrc = [byte[]]($smp + $crcHi + $crcLo)             # SMP (9) + CRC (2) = 11 bytes
# NLIP decoded frame: [2-byte BE length of (SMP+CRC)] + [SMP] + [CRC]
$lenHi  = [byte](($smpCrc.Length -shr 8) -band 0xFF)   # 0x00
$lenLo  = [byte]($smpCrc.Length -band 0xFF)             # 0x0B
$frame  = [byte[]]([byte[]]($lenHi, $lenLo) + $smpCrc) # length(2) + SMP(9) + CRC(2) = 13 bytes

# NLIP packet: 0x06 0x09 + base64(payload+CRC) + newline
$b64    = [Convert]::ToBase64String($frame)
$packet = [byte[]]([byte[]](0x06, 0x09) + [Text.Encoding]::ASCII.GetBytes($b64) + [byte[]](0x0A))

Write-Host "Sending: 06 09 $b64 <LF>"

$sp = New-Object System.IO.Ports.SerialPort($PORT, $BAUD, 'None', 8, 'One')
$sp.ReadTimeout = 2000
try {
    $sp.Open()
} catch {
    Write-Host "ERROR: Could not open $PORT — $_"
    exit 1
}
$sp.BaseStream.Write($packet, 0, $packet.Length)
Start-Sleep -Milliseconds 500

try {
    $buf = [byte[]]::new(512)
    $n   = $sp.Read($buf, 0, $buf.Length)
    $hex = ($buf[0..($n-1)] | ForEach-Object { '{0:X2}' -f $_ }) -join ' '
    Write-Host "Response ($n bytes): $hex"
    # The response is also NLIP: 06 09 <base64> 0A
    # Strip 06 09 prefix and decode base64 to see raw CBOR
    if ($n -gt 3 -and $buf[0] -eq 0x06 -and $buf[1] -eq 0x09) {
        $b64resp = [Text.Encoding]::ASCII.GetString($buf[2..($n-2)])  # trim 06 09 and newline
        $decoded = [Convert]::FromBase64String($b64resp)
        Write-Host "Decoded SMP+CRC ($($decoded.Length) bytes): $(($decoded | ForEach-Object { '{0:X2}' -f $_ }) -join ' ')"
    }
} catch { Write-Host "Timeout — no response" }

$sp.Close()
