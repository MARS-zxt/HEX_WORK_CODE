$ErrorActionPreference = "Stop"

try {
    $client = New-Object System.Net.Sockets.TcpClient
    $client.Connect("127.0.0.1", 9876)
    Write-Host "=== Testing SET_DATA ===" -ForegroundColor Green

    $stream = $client.GetStream()
    $writer = New-Object System.IO.StreamWriter($stream)
    $reader = New-Object System.IO.StreamReader($stream)
    $writer.AutoFlush = $true

    # Send test data to UI display
    $writer.WriteLine("SET_DATA open_time=2.20 open_upper=150.0 open_lower=10.0 close_time=2.40 close_upper=140.0 close_lower=8.0")
    Start-Sleep -Milliseconds 100
    $resp = $reader.ReadLine()
    Write-Host "SET_DATA: $resp"

    # Verify with GET_DATA
    $writer.WriteLine("GET_DATA")
    Start-Sleep -Milliseconds 50
    $resp = $reader.ReadLine()
    Write-Host "GET_DATA: $resp"

    $client.Close()
    Write-Host "=== Test Complete ===" -ForegroundColor Green
} catch {
    Write-Host "FAILED: $($_.Exception.Message)" -ForegroundColor Red
}
