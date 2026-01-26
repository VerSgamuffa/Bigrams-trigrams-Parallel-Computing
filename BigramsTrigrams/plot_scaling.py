import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("scaling_results.csv")
threads = df["threads"]

char_speedup = df["char_seq"] / df["char_par"]
word_speedup = df["word_seq"] / df["word_par"]

#speedup graph
plt.figure(figsize=(8,5))
plt.plot(threads, char_speedup, marker='o', label="Character speedup")
plt.plot(threads, word_speedup, marker='o', label="Word speedup")
plt.xlabel("Number of threads")
plt.ylabel("Speedup")
plt.title("Speedup Character vs Word")
plt.grid(True)
plt.legend()
plt.savefig("speedup.png", dpi=300)

#character graph
plt.figure(figsize=(8,5))
plt.plot(threads, df["char_seq"], marker='o', label="Character n-grams")
plt.plot(threads, df["char_par"], marker='o', label="Character parallel")
plt.xlabel("Number of threads")
plt.ylabel("Time (s)")
plt.title("Character Execution Time")
plt.grid(True)
plt.legend()
plt.savefig("execution_character.png", dpi=300)

#parallel graph
plt.figure(figsize=(8,5))
plt.plot(threads, df["word_seq"], marker='o', label="Word n-grams")
plt.plot(threads, df["word_par"], marker='o', label="Word parallel")
plt.xlabel("Number of threads")
plt.ylabel("Time (s)")
plt.title("Word Execution Time")
plt.grid(True)
plt.legend()
plt.savefig("execution_word.png", dpi=300)

print("Plots saved: speedup.png, execution_word.png, execution_character.png")
plt.show()
