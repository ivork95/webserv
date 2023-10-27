import unittest
import requests
import http.client


class TestServerResponse(unittest.TestCase):
    def test_get(self):
        url = "http://localhost:8081"

        response = requests.get(
            url=url
        )
        self.assertEqual(response.status_code, 200)

    def test_get_not_found(self):
        url = "http://localhost:8081/doesntexist"

        response = requests.get(
            url=url
        )
        self.assertEqual(response.status_code, 404)

    def test_post(self):
        url = "http://localhost:8081/upload"
        files = {'file': open('Poster.jpg', 'rb')}
        headers = {
            'Content-Length': '1046495',
        }

        response = requests.post(
            url=url,
            headers=headers,
            files=files,
        )
        files["file"].close()
        self.assertTrue(response.history)
        self.assertEqual(response.history[0].status_code, 303)

    def atest_delete(self):
        filename = '../www/upload/test.txt'

        try:
            with open(filename, 'x') as file:
                pass
        except FileExistsError:
            pass

        url = f"http://localhost:8081/upload/{filename}"
        response = requests.delete(
            url=url
        )
        self.assertEqual(response.status_code, 200)

    def test_delete_method_not_allowed(self):
        url = "http://localhost:8081/images/test"
        response = requests.delete(
            url=url
        )
        self.assertEqual(response.status_code, 405)

    def test_post_chunked(self):
        conn = http.client.HTTPConnection("localhost:8081")

        conn.putrequest("POST", "/upload")
        conn.putheader("Transfer-Encoding", "chunked")
        conn.endheaders()

        # Write the chunked data
        conn.send(b"7\r\nMozilla\r\n11\r\nDeveloper Network\r\n0\r\n\r\n")

        # Get the response
        response = conn.getresponse()
        conn.close()
        self.assertEqual(response.status, 200)
        # print(response.status, response.reason)
        # print(response.read())


if __name__ == '__main__':
    unittest.main()
