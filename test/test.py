import unittest
import requests
import http.client


class TestServerResponse(unittest.TestCase):
    def setUp(self):
        self.base_url = "http://localhost:8081"

    def test_get(self):
        url = f"{self.base_url}"
        response = requests.get(url=url)
        self.assertEqual(response.status_code, 200)

    def test_get_not_found(self):
        url = f"{self.base_url}/doesntexist"
        response = requests.get(url=url)
        self.assertEqual(response.status_code, 404)

    def test_get_method_not_allowed(self):
        url = f"{self.base_url}/no-get"
        response = requests.get(url=url)
        self.assertEqual(response.status_code, 405)

    def test_get_autoindex(self):
        url = f"{self.base_url}/autoindex/"
        response = requests.get(url=url)
        self.assertEqual(response.status_code, 200)

    def test_post(self):
        url = f"{self.base_url}/upload"
        files = {'file': open('Poster.jpg', 'rb')}
        headers = {'Content-Length': '1046495'}

        response = requests.post(url=url, headers=headers, files=files)
        files["file"].close()
        self.assertTrue(response.history)
        self.assertEqual(response.history[0].status_code, 303)

    def test_post_bigger_max_client_size(self):
        url = f"{self.base_url}/post-bigger"
        large_payload = 'A' * 1024
        headers = {'Content-Length': str(len(large_payload))}

        response = requests.post(url=url, headers=headers, data=large_payload)
        self.assertEqual(response.status_code, 413)

    def test_post_method_not_allowed(self):
        url = f"{self.base_url}/no-post"
        payload = 'A'
        headers = {'Content-Length': str(len(payload))}

        response = requests.post(url=url, headers=headers, data=payload)
        self.assertEqual(response.status_code, 405)

    def test_post_chunked(self):
        conn = http.client.HTTPConnection("localhost:8081")
        conn.putrequest("POST", "/upload")
        conn.putheader("Transfer-Encoding", "chunked")
        conn.endheaders()

        conn.send(b"7\r\nMozilla\r\n11\r\nDeveloper Network\r\n0\r\n\r\n")
        response = conn.getresponse()
        conn.close()
        self.assertEqual(response.status, 200)

    def test_post_invalid_chunks(self):
        conn = http.client.HTTPConnection("localhost:8081")
        conn.putrequest("POST", "/upload")
        conn.putheader("Transfer-Encoding", "chunked")
        conn.endheaders()

        conn.send(b"9\r\nMozilla\r\n11\r\nDeveloper Network\r\n0\r\n\r\n")
        response = conn.getresponse()
        conn.close()
        self.assertEqual(response.status, 500)

    def test_delete(self):
        filename = 'test.txt'
        file_path = f"../www/delete/{filename}"
        with open(file_path, 'w') as file:
            file.write("Test content")

        url = f"{self.base_url}/delete/{filename}"
        response = requests.delete(url=url)
        self.assertEqual(response.status_code, 200)

    def test_delete_method_not_allowed(self):
        url = f"{self.base_url}/no-delete/test"
        response = requests.delete(url=url)
        self.assertEqual(response.status_code, 405)

    def test_delete_file_does_not_exist(self):
        url = f"{self.base_url}/delete/does_not_exist"
        response = requests.delete(url=url)
        self.assertEqual(response.status_code, 500)

        # TO ADD
        # test UNKNOWN command
        # ...
        # ...

    def delete_tests(self):
        self.test_delete()
        self.test_delete_file_does_not_exist()
        self.test_delete_method_not_allowed()

    def post_tests(self):
        self.test_post()
        self.test_post_bigger_max_client_size()
        self.test_post_method_not_allowed()
        self.test_post_chunked()
        self.test_post_invalid_chunks()

    def get_tests(self):
        self.test_get()
        self.test_get_not_found()
        self.test_get_method_not_allowed()
        self.test_get_autoindex()


if __name__ == '__main__':
    unittest.main()
