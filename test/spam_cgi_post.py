import requests

cookies = {
    '_ga': 'GA1.1.1853335373.1693823153',
    '_ga_M78MKT6JKY': 'GS1.1.1693823153.1.1.1693823965.0.0.0',
    '_ga_XNTP0G5VDT': 'GS1.1.1696850402.1.1.1696853451.0.0.0',
}

headers = {
    'Accept': 'text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7',
    'Accept-Language': 'nl-NL,nl;q=0.9,en-US;q=0.8,en;q=0.7',
    'Cache-Control': 'no-cache',
    'Connection': 'keep-alive',
    # 'Content-Length': '0',
    'Content-Type': 'application/x-www-form-urlencoded',
    # 'Cookie': '_ga=GA1.1.1853335373.1693823153; _ga_M78MKT6JKY=GS1.1.1693823153.1.1.1693823965.0.0.0; _ga_XNTP0G5VDT=GS1.1.1696850402.1.1.1696853451.0.0.0',
    'DNT': '1',
    'Origin': 'http://localhost:8081',
    'Pragma': 'no-cache',
    'Referer': 'http://localhost:8081/',
    'Sec-Fetch-Dest': 'document',
    'Sec-Fetch-Mode': 'navigate',
    'Sec-Fetch-Site': 'same-origin',
    'Sec-Fetch-User': '?1',
    'Upgrade-Insecure-Requests': '1',
    'User-Agent': 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/117.0.0.0 Safari/537.36',
    'sec-ch-ua': '"Google Chrome";v="117", "Not;A=Brand";v="8", "Chromium";v="117"',
    'sec-ch-ua-mobile': '?0',
    'sec-ch-ua-platform': '"macOS"',
}

for i in range(1000):
    response = requests.post(
        'http://localhost:8081/cgi-bin', cookies=cookies, headers=headers)
    print(response.status_code)
