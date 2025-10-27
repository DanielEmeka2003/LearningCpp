for i in range(10, 1, -1):
  print(i)

def shift_string(S: str , queries):
  N: int = S.__len__()
  S_array = list(S) # Convert string to character array

  for query in queries:
    Left, Right, value = query
    Left -= 1 # 0-based indexing
    Right -= 1

    for i in range(Left, Right + 1):
      ascii_val = ord(S_array[i])
      shifted_ascii = ascii_val + value

      if shifted_ascii > ord('z'):
        shifted_ascii = (shifted_ascii - ord('z') - 1) + ord('a')

      S_array[i] = chr(shifted_ascii)
    print("".join(S_array))

  return "".join(S_array) # Convert character array back to string

# Example usage:
S = "abcdefghij"
queries = [
  [2, 5, 2],
  [4, 7, 3],
  [1, 10, 1]
]

result = shift_string(S, queries)
print(result) # Output: bcdzab