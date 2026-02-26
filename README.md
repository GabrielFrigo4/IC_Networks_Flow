# IC | Problemas de Fluxos em Redes: Teoria, Algoritmos e Implementações
 [![Baixar PDF][baixar_pdf_icon]][baixar_pdf_ic]

## 📌 Sobre o Projeto
 Este repositório contém os materiais e códigos desenvolvidos para o projeto de Iniciação Científica (Edital: 01/2025 PIC/PIBIC/PIBITI/PIBIC-AF) focado na interseção entre Ciência da Computação e Matemática Discreta.

 Fluxos em digrafos constituem uma área fundamental da Otimização Combinatória, com amplas aplicações práticas em redes de transporte e alocação de recursos. Este projeto adota uma abordagem que combina fundamentos teóricos, implementação algorítmica e validação experimental para o estudo de problemas de fluxo máximo e fluxo de custo mínimo.

## 📖 Estrutura do Projeto
 O projeto é conduzido através de ciclos de estudo e implementação, dividindo-se nas seguintes frentes:

 * **Fundamentos Teóricos e Modelagem:** Estudo rigoroso de fluxos em redes, explorando teoremas fundamentais e a modelagem de diversos problemas (como emparelhamento máximo em grafos bipartidos e empacotamento de caminhos disjuntos).
 * **Implementação Algorítmica:** Desenvolvimento eficiente em linguagem C++ utilizando estruturas de dados adequadas para a representação de grafos. Entre os algoritmos estudados e desenvolvidos estão Ford-Fulkerson, Edmonds-Karp, Dinic, Goldberg-Tarjan e o Network Simplex.
 * **Validação Experimental:** Condução de testes práticos utilizando instâncias padronizadas de benchmarks reconhecidos, como as coleções DIMACS. A validação conta com uma análise comparativa de desempenho focada em métricas de tempo de execução.

---

## 💻 Implementações e Códigos
 Abaixo estão listados os algoritmos já implementados no escopo deste projeto:

 * 🚀 **[Edmonds-Karp][edmonds_karp]**
 * 🚀 **[Edmonds-Karp][push_relabel]**
 * 🚀 **[Edmonds-Karp][push_relabel_faster]**
 * 🚀 **[Edmonds-Karp][dinic_algorithm]**
 * 🚀 **[Edmonds-Karp][mpm_algorithm]**

 *(Outros algoritmos previstos no cronograma, como Dinic, Goldberg-Tarjan e Network Simplex, serão adicionados nesta seção em breve).*

## 🛠️ Como Compilar o Código-Fonte (LaTeX)
 Caso queira gerar o projeto de pesquisa (`ic.pdf`) localmente a partir do código-fonte `ic.tex`:
 1. Certifique-se de ter uma distribuição LaTeX instalada (como TeX Live ou MiKTeX) com suporte aos pacotes requeridos, como `amsmath`, `tikz`, e `geometry`.
 2. Clone este repositório em sua máquina.
 3. Compile o arquivo `.tex` principal utilizando o compilador `pdflatex` (e o `bibtex` para referências bibliográficas).

## ✍️ Autor
 * **Gabriel Frigo** - Pesquisador de Iniciação Científica

[edmonds_karp]: https://github.com/GabrielFrigo4/IC_Networks_Flow/Implementações/1.%20Edmonds-Karp/README.md
[push_relabel]: https://github.com/GabrielFrigo4/IC_Networks_Flow/Implementações/2.%20Push-Relabel/README.md
[push_relabel_faster]: https://github.com/GabrielFrigo4/IC_Networks_Flow/Implementações/3.%20Push-Relabel%20Improved/README.md
[dinic_algorithm]: https://github.com/GabrielFrigo4/IC_Networks_Flow/Implementações/4.%20Dinic's%20Algorithm/README.md
[mpm_algorithm]: https://github.com/GabrielFrigo4/IC_Networks_Flow/Implementações/5.%20MPM%20algorithm/README.md

[baixar_pdf_ic]: https://github.com/GabrielFrigo4/IC_Networks_Flow/releases/latest/download/ic.pdf
[baixar_pdf_icon]: https://img.shields.io/badge/PDF-Baixar-red?style=for-the-badge&logo=adobeacrobatreader