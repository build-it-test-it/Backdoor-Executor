import UIKit

class AppListViewController: UIViewController {
    
    // MARK: - Properties
    
    private let jitService: JITService
    private let tableView: UITableView = {
        let tableView = UITableView()
        tableView.register(AppCell.self, forCellReuseIdentifier: AppCell.identifier)
        tableView.translatesAutoresizingMaskIntoConstraints = false
        return tableView
    }()
    
    private let searchController = UISearchController(searchResultsController: nil)
    private var apps: [App] = []
    private var filteredApps: [App] = []
    
    // MARK: - Initialization
    
    init(jitService: JITService) {
        self.jitService = jitService
        super.init(nibName: nil, bundle: nil)
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    // MARK: - Lifecycle Methods
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        setupUI()
        setupTableView()
        setupSearchController()
        loadApps()
    }
    
    // MARK: - UI Setup
    
    private func setupUI() {
        view.backgroundColor = .systemBackground
        title = "Select App"
        
        // Add subviews
        view.addSubview(tableView)
        
        // Set up constraints
        NSLayoutConstraint.activate([
            tableView.topAnchor.constraint(equalTo: view.safeAreaLayoutGuide.topAnchor),
            tableView.leadingAnchor.constraint(equalTo: view.leadingAnchor),
            tableView.trailingAnchor.constraint(equalTo: view.trailingAnchor),
            tableView.bottomAnchor.constraint(equalTo: view.safeAreaLayoutGuide.bottomAnchor)
        ])
    }
    
    private func setupTableView() {
        tableView.delegate = self
        tableView.dataSource = self
    }
    
    private func setupSearchController() {
        searchController.searchResultsUpdater = self
        searchController.obscuresBackgroundDuringPresentation = false
        searchController.searchBar.placeholder = "Search Apps"
        navigationItem.searchController = searchController
        definesPresentationContext = true
    }
    
    // MARK: - Data Loading
    
    private func loadApps() {
        // In a real implementation, this would get the actual installed apps
        // For now, we'll use the AppManager to get a list of compatible apps
        apps = AppManager.shared.getJITCompatibleApps()
        filteredApps = apps
        tableView.reloadData()
    }
    
    // MARK: - Filtering
    
    private func filterApps(for searchText: String) {
        filteredApps = apps.filter { app in
            return app.name.lowercased().contains(searchText.lowercased()) ||
                   app.bundleId.lowercased().contains(searchText.lowercased())
        }
        
        tableView.reloadData()
    }
    
    // MARK: - JIT Enablement
    
    private func enableJITForApp(_ app: App) {
        let loadingIndicator = showLoadingIndicator()
        
        jitService.enableJIT(for: app) { [weak self] result in
            guard let self = self else { return }
            
            self.hideLoadingIndicator(indicator: loadingIndicator)
            
            switch result {
            case .success(let response):
                // Save app to recent apps
                AppManager.shared.saveToRecentApps(app)
                
                // Show success message
                self.showAlert(title: "JIT Enabled", message: response.message) {
                    // Launch the app
                    if !self.jitService.launchApp(withBundleId: app.bundleId) {
                        self.showAlert(title: "Launch Failed", message: "Could not launch the app. Please open it manually.")
                    }
                    
                    // Navigate back to home screen
                    self.navigationController?.popViewController(animated: true)
                }
                
            case .failure(let error):
                self.showError(error)
            }
        }
    }
}

// MARK: - UITableViewDelegate, UITableViewDataSource

extension AppListViewController: UITableViewDelegate, UITableViewDataSource {
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return filteredApps.count
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        guard let cell = tableView.dequeueReusableCell(withIdentifier: AppCell.identifier, for: indexPath) as? AppCell else {
            return UITableViewCell()
        }
        
        let app = filteredApps[indexPath.row]
        cell.configure(with: app)
        
        return cell
    }
    
    func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        tableView.deselectRow(at: indexPath, animated: true)
        
        let app = filteredApps[indexPath.row]
        
        showConfirmation(
            title: "Enable JIT",
            message: "Do you want to enable JIT for \(app.name)?",
            yesHandler: { [weak self] in
                self?.enableJITForApp(app)
            }
        )
    }
    
    func tableView(_ tableView: UITableView, heightForRowAt indexPath: IndexPath) -> CGFloat {
        return 70
    }
}

// MARK: - UISearchResultsUpdating

extension AppListViewController: UISearchResultsUpdating {
    func updateSearchResults(for searchController: UISearchController) {
        guard let searchText = searchController.searchBar.text, !searchText.isEmpty else {
            filteredApps = apps
            tableView.reloadData()
            return
        }
        
        filterApps(for: searchText)
    }
}